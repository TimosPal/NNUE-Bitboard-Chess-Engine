#include "Search.h"

#include <algorithm>

#include "NNUE.h"

namespace ChessEngine {

    int NegaMax(const Board& board, int depth, int a, int b) {
        auto moves = board.GetLegalMoves();
        if(moves.empty()){
            // Terminal node.
            GameResult result = board.Result(moves);
            switch(result){
                case GameResult::WhiteWon:
                case GameResult::BlackWon:
                    // The board is flipped. If the game is over
                    // the current side lost. We return relative to
                    // the current side hence the score is negative.
                    return -(INT16_MAX + depth);
                case GameResult::Draw:
                    return 0;
                case GameResult::Playing:
                    assert(false);
                    break;
            }
        }
        if (depth <= 0) {
            return Evaluate(board);
        }

        for (auto move : moves) {
            Board new_board = Board(board);
            new_board.PlayMove(move);
            new_board.Mirror();

            int score = -NegaMax(new_board, depth - 1, -b, -a);
            if( score >= b)
                return b;
            if( score > a )
                a = score;
        }

        return a;
    }

    Move GetBestMove(const Board& board, int depth){
        Move best_move;
        int value = INT16_MIN;

        auto moves = board.GetLegalMoves();
        for (auto move : moves) {
            Board new_board = Board(board);
            new_board.PlayMove(move);
            new_board.Mirror();

            // Using 16 bits because 32 overflows.
            int a = INT16_MIN;
            int b = INT16_MAX;
            int score = -NegaMax(new_board, depth - 1, -b, -a);
            if(score > value){
                value = score;
                best_move = move;
            }
        }

        return best_move;
    }

}