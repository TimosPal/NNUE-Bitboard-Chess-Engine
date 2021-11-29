#include "Search.h"

#include <algorithm>

#include "NNUE.h"

namespace ChessEngine {

    int search_nodes = 0;

    void SortMoves(MoveList moves){
        std::sort(moves.begin(), moves.end(), [](){

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

        auto moves = board.GetLegalCaptures();
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

    int NegaMax(const Board& board, int depth, int a, int b) {
        search_nodes++;
        if (depth == 0) {
            return QSearch(board, a, b);
        }
        auto moves = board.GetLegalMoves();
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
        for (const auto& move : moves) {
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

        std::cout << search_nodes / 1000000.0f << std::endl;

        return best_move;
    }

    int Perft(const Board& board, int depth) {
        int nodes = 0;

        if (depth == 0)
            return 1ULL;

        MoveList moves = board.GetLegalMoves();
        for (const Move& move : moves) {
            Board temp = board;
            temp.PlayMove(move);
            temp.Mirror();

            nodes += Perft(temp, depth - 1);
        }

        return nodes;
    }

}