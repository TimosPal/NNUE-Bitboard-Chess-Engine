#ifndef SEARCH_H
#define SEARCH_H

#include "Board.h"

#include <algorithm>

#include "NNUE.h"

namespace ChessEngine {


    int MinMax(Board board, int depth, bool maximizing_player){
        if (depth <= 0) {
            return Evaluate(board);
        }

        if(maximizing_player){
            int value = INT32_MIN;
            auto moves = board.GetLegalMoves();
            for(auto move : moves){
                Board new_board = Board(board);
                new_board.PlayMove(move);
                new_board.Mirror();

                value = std::max(value, MinMax(new_board, depth - 1, false));
            }
            return value;
        }else{
            int value = INT32_MAX;
            auto moves = board.GetLegalMoves();
            for(auto move : moves){
                Board new_board = Board(board);
                new_board.PlayMove(move);
                new_board.Mirror();

                value = std::min(value, MinMax(new_board, depth - 1, true));
            }
            return value;
        }
    }

    int NegaMax(const Board& board, int depth, int a, int b, int color) {
        auto moves = board.GetLegalMoves();
        if(moves.empty()){
            // Terminal node.
            GameResult result = board.Result(moves);
            switch(result){
                case GameResult::WhiteWon:
                case GameResult::BlackWon:
                    return (INT16_MAX + depth) * color;
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

        int value = INT16_MIN;
        for (auto move : moves) {
            Board new_board = Board(board);
            new_board.PlayMove(move);
            new_board.Mirror();
            value = std::max(value, -NegaMax(new_board, depth - 1, -b, -a, -color));
            a = std::max(a, value);
            if (a >= b) break;
        }

        return value;
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
            int color = 1;
            int score = -NegaMax(new_board, depth - 1, -b, -a, -color);
            if(score > value){
                value = score;
                best_move = move;
            }
        }

        std::cout << "Score : " << value << std::endl;
        return best_move;
    }

}

#endif