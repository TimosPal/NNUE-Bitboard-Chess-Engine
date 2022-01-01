#include "MCTS.h"

#include <vector>
#include <cmath>

namespace ChessEngine::MCTS{
    struct Node{
        bool is_terminal;
        float score{};
        int visits{};
        Board state;
        Move move;
        std::vector<Node> children;

        Node(Board board, bool is_terminal, Move move = Move()){
            this->state = board;
            this->is_terminal = is_terminal;
            this->move = move;
        }
    };

    float UCT(const Node& node, Node* parent) {
        static float c = 1;
        static float e = 1;

        int parent_visits = 0;
        if (parent) {
            for (auto &child : parent->children) {
                parent_visits += child.visits;
            }
        }
        parent_visits -= node.visits;

        return node.score / (e + node.visits) + c * sqrtf(logf(e + parent_visits) / (e + node.visits));
    }

    int Rollout(Board board, bool for_white){
        int modifier = for_white ? 1 : -1;
        while(true) {
            ChessEngine::Bitboard pins = board.GetPins();
            bool is_in_check = board.IsInCheck();
            auto moves = board.GetLegalCaptures(pins, is_in_check);
            auto quiet_moves = board.GetLegalQuietMoves(pins, is_in_check);
            moves.insert(moves.end(), quiet_moves.begin(), quiet_moves.end());

            ChessEngine::GameResult result = board.Result(moves);
            if (result != ChessEngine::GameResult::Playing) {
                if (board.IsFlipped())
                    board.Mirror();
                if (result == ChessEngine::GameResult::WhiteWon) {
                    return 1 * modifier;
                } else if (result == ChessEngine::GameResult::BlackWon) {
                    return -1 * modifier;
                } else {
                    return 0;
                }
            }

            ChessEngine::Move move = moves[rand() % moves.size()];
            board.PlayMove(move);
            board.Mirror();
        }
    }

    float Sample(Node& node, bool for_white){
        node.visits++;
        if(node.children.empty()){
            if(node.is_terminal)
                return 0;

            // Expansion phase.
            const Board& board = node.state;

            Bitboard pins = board.GetPins();
            bool is_in_check = board.IsInCheck();
            MoveList moves = board.GetLegalCaptures(pins, is_in_check);
            MoveList quiet_moves = board.GetLegalQuietMoves(pins, is_in_check);
            moves.insert( moves.end(), quiet_moves.begin(), quiet_moves.end());

            for (const Move& move : moves) {
                Board temp = board;
                temp.PlayMove(move);
                temp.Mirror();

                ChessEngine::GameResult result = board.Result(moves);
                bool terminal = result != GameResult::Playing;

                Node new_node(temp, terminal, move);
                node.children.push_back(new_node);
            }

            // Rollout phase.
            int score;
            GetBestMove(board, 3, score);

            float rollout_result = Rollout(board, for_white);
            node.score += rollout_result;
            return rollout_result;
        }else{
            // Selection phase.
            float max_uct = INT32_MIN;
            Node* max_uct_node = nullptr;
            for(auto& child : node.children) {
                float uct_score = UCT(child, &node);
                if(uct_score > max_uct){
                    max_uct = uct_score;
                    max_uct_node = &child;
                }
            }

            // Back propagate.
            node.score += Sample(*max_uct_node, for_white);
        }

        return node.score;
    }

    Move Search(const Board& state, int iterations, bool for_white){
        Node root(state, false);
        for (int i = 0; i < iterations; i++) {
            Sample(root, for_white);
        }

        int max = INT32_MIN;
        Move best_move;
        for (const auto& child : root.children) {
            if(child.visits > max){
                max = child.visits;
                best_move = child.move;
            }
        }
        return best_move;
    }
}