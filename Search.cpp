#include "Search.h"

#include <algorithm>
#include <random>

#include "NNUE.h"
#include "TranspositionTable.h"

namespace ChessEngine {

    TranspositionTable transposition_table;

    int search_nodes = 0;

    static int GetMVVScore(const PieceType& own_type, const PieceType& enemy_type){
        int base_score = 0;
        switch (enemy_type) {
            case King:
                assert(false);
                base_score = 600;
                break;
            case Queen:
                base_score = 500;
                break;
            case Rook:
                base_score = 400;
                break;
            case Bishop:
                base_score = 300;
                break;
            case Knight:
                base_score = 200;
                break;
            case Pawn:
            case None: // Only sorts captures . so none means en passant.
                base_score = 100;
                break;
        }

        switch (own_type) {
            case King:
                base_score += 0;
                break;
            case Queen:
                base_score += 1;
                break;
            case Rook:
                base_score += 2;
                break;
            case Bishop:
                base_score += 3;
                break;
            case Knight:
                base_score += 4;
                break;
            case Pawn:
                base_score += 5;
                break;
            case None:
                base_score += 0;
                assert(false);
                break;
        }

        return base_score;
    }

    void SortMoves(const Board& board, MoveList& moves){
        auto move_score = [=] (const Move& move){
            PieceType type_own, type_enemy;
            {
                auto[file_from, rank_from] = move.GetFrom().GetCoords();
                type_own = board.GetPieceTypeAt(file_from, rank_from);
                auto[file_to, rank_to] = move.GetTo().GetCoords();
                type_enemy = board.GetPieceTypeAt(file_to, rank_to);
            }
            return GetMVVScore(type_own, type_enemy);
        };

        std::stable_sort(moves.begin(), moves.end(), [=](const Move& mv1, const Move& mv2){
            return move_score(mv1) > move_score(mv2);
        });
    }

    int SimpleEval(const Board& board){
        int own = board.GetRepresentation().own_pieces.Count();
        int enemy = board.GetRepresentation().enemy_pieces.Count();
        return own - enemy;
    }

    int QSearch(const Board& board, int a, int b) {
        search_nodes++;

        int best_score = NNUE::Instance().EvaluateIncremental(board);
        assert(best_score == NNUE::Instance().Evaluate(board));
        if(best_score >= b)
            return b;
        if(best_score > a)
            a = best_score;

        MoveList moves = board.GetLegalCaptures();
        SortMoves(board, moves);

        for (const auto& move : moves) {
            // Only capture moves.
            Board new_board = Board(board);
            new_board.PlayMove(move);
            new_board.Mirror();

            int score = -QSearch(new_board, -b, -a);
            if(score > best_score)
                best_score = score;
            if (score >= b)
                return b;
            if (score > a)
                a = score;
        }

        return best_score;
    }

    int PVSearch(const Board& board, int depth, int starting_depth, int a, int b, Move& best_move) {
        search_nodes++;

        if (depth <= 0) {
            return QSearch(board, a, b);
        }

        uint64_t zobrist_key = board.GetZobristKey();
        uint8_t tree_level = starting_depth - depth;
        bool is_root = tree_level == 0;

        TTEntry entry_result;
        bool entry_found = transposition_table.GetEntry(zobrist_key, entry_result);
        if(entry_found && !is_root && entry_result.depth >= depth){
            if(entry_result.type == NodeType::Exact){
                return entry_result.evaluation;
            }else if(entry_result.type == NodeType::Alpha && entry_result.evaluation <= a){
                return a;
            }else if(entry_result.type == NodeType::Beta && entry_result.evaluation >= b){
                return b;
            }
        }

        MoveList moves = board.GetLegalCaptures();
        SortMoves(board, moves);
        MoveList quiet_moves = board.GetLegalQuietMoves();
        moves.insert(moves.end(), quiet_moves.begin(), quiet_moves.end());

        // Put TT move first.
        if(entry_found) {
            // TT's move can be invalid if it was never set. This case isnt troublesome since
            // it will not be found in the legal moves list.
            Move tt_move = entry_result.best_move;
            auto pivot = std::find(moves.begin(), moves.end(), tt_move);
            if (pivot != moves.end()) {
                std::rotate(moves.begin(), pivot, pivot + 1);
            }
        }

        GameResult game_result = board.Result(moves);
        switch(game_result){
            // Terminal node.
            case GameResult::WhiteWon:
            case GameResult::BlackWon:
                // The board is flipped. If the game is over
                // the current side lost. We return relative to
                // the current side hence the score is negative.
                return -(INT16_MAX + depth);
            case GameResult::Draw:
                return 0;
            case GameResult::Playing:
                break;
        }

        NodeType node_type = NodeType::Alpha;
        Move current_best_move;
        bool pv_search = true;
        int best_score = INT32_MIN;
        for (const auto& move : moves) {
            Board new_board = Board(board);
            new_board.PlayMove(move);
            new_board.Mirror();

            int score;
            if (pv_search) {
                score = -PVSearch(new_board, depth - 1, starting_depth, -b, -a, best_move);
            } else {
                score = -PVSearch(new_board, depth - 1, starting_depth, -a - 1, -a, best_move);
                if (score > a && score < b) {
                    score = -PVSearch(new_board, depth - 1, starting_depth, -b, -a, best_move);
                }
            }

            if(score > best_score){
                best_score = score;
                current_best_move = move;
            }
            if(score >= b) {
                node_type = NodeType::Beta;
                break;
            }
            if(score > a) {
                node_type = NodeType::Exact;
                pv_search = false;
                a = score;

                if(is_root)
                    best_move = move;
            }
        }

        transposition_table.AddEntry(zobrist_key, TTEntry(depth, best_score, node_type, current_best_move));
        return best_score;
    }

    Move GetBestMove(const Board& board, int depth){
        search_nodes = 0;

        // Iterative deepening.
        int a = 2 * INT16_MIN;
        int b = 2 * INT16_MAX;
        Move best_move;
        for (int current_depth = 1; current_depth <= depth; current_depth++) {
            // Using 16 bits because 32 overflows.
            int eval = PVSearch(board, current_depth, current_depth, a, b, best_move);
            //std::cout << "evaluation : " << eval << std::endl;

            // Aspiration search
            /*if(eval <= a || eval >= b) {
                a = 2 * INT16_MIN;
                b = 2 * INT16_MAX;
                current_depth--;
                continue;
            }

            a = eval - 100;
            b = eval + 100;*/
        }

        //std::cout << "Nodes : " << search_nodes / 1000000.0f << std::endl;
        return best_move;
    }

    int Perft(const Board& board, int depth) {
        int nodes = 0;

        if (depth == 0)
            return 1ULL;

        MoveList moves = board.GetLegalCaptures();
        MoveList quiet_moves = board.GetLegalQuietMoves();
        moves.insert( moves.end(), quiet_moves.begin(), quiet_moves.end());
        for (const Move& move : moves) {
            Board temp = board;
            temp.PlayMove(move);
            temp.Mirror();

            nodes += Perft(temp, depth - 1);
        }

        return nodes;
    }

}