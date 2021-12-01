#include "Search.h"

#include <algorithm>

#include "NNUE.h"

namespace ChessEngine {

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

        //int evaluation = Evaluate(board);
        //int evaluation = SimpleEval(board);
        int evaluation = EvaluateIncremental(board);
        if(evaluation >= b)
            return b;
        if(evaluation > a)
            a = evaluation;

        MoveList moves = board.GetLegalCaptures();
        SortMoves(board, moves);

        for (const auto& move : moves) {
            // Only capture moves.
            Board new_board = Board(board);
            new_board.PlayMove(move);
            new_board.Mirror();

            int score = -QSearch(new_board, -b, -a);
            if (score >= b)
                return b;
            if (score > a)
                a = score;
        }

        return a;
    }

    int NegaMax(const Board& board, int depth, int starting_depth, int a, int b, Move& best_move) {
        search_nodes++;
        if (depth == 0) {
            return QSearch(board, a, b);
        }

        MoveList moves = board.GetLegalCaptures();
        SortMoves(board, moves);
        MoveList quiet_moves = board.GetLegalQuietMoves();
        moves.insert(moves.end(), quiet_moves.begin(), quiet_moves.end());

        GameResult result = board.Result(moves);
        switch(result){
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

        for (const auto& move : moves) {
            Board new_board = Board(board);
            new_board.PlayMove(move);
            new_board.Mirror();

            int score = -NegaMax(new_board, depth - 1, starting_depth, -b, -a, best_move);
            if( score >= b)
                return b;
            if(score > a ) {
                a = score;
                if(depth == starting_depth)
                    best_move = Move(move);
            }
        }

        return a;
    }

    Move GetBestMove(const Board& board, int depth){
        // Using 16 bits because 32 overflows.
        int a = 2*INT16_MIN;
        int b = 2*INT16_MAX;
        Move best_move;
        int eval = NegaMax(board, depth, depth, a, b, best_move);

        std::cout << "Nodes : " << search_nodes / 1000000.0f << std::endl;
        std::cout << "evaluation : " << eval << std::endl;

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