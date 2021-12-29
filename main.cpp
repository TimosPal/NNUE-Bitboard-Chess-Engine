#include <iostream>

#include "AttackTables.h"
#include "FenParser.h"
#include "NNUE.h"
#include "Search.h"
#include "ZobristKey.h"
#include "UCI.h"

int main() {
    {
        PROFILE_SCOPE("Program");
        ChessEngine::AttackTables::InitMoveTables();
        ChessEngine::NNUE::InitModel("nn-62ef826d1a6d.nnue");
        ChessEngine::Zobrist::InitZobristKeysArrays();
/*        ChessEngine::UCI::MainLoop();
        return 0;*/

        std::string fen = "r2rk3/pp1b1ppN/2n1p2p/4P3/q2p3Q/3B4/5PPP/3RR1K1 w - - 10 25";

        ChessEngine::Board::BoardInfo info = {};
        IF_ERROR(!ChessEngine::ParseFenString(fen, info), "Invalid fen string.")
        ChessEngine::Board board(info);

        std::cout << GetBestMove(board, 5) << std::endl;

        int i = 1;
        while(true) {
            auto moves = board.GetLegalCaptures();
            auto quiet_moves = board.GetLegalQuietMoves();
            moves.insert(moves.end(), quiet_moves.begin(), quiet_moves.end());

            ChessEngine::GameResult result = board.Result(moves);
            if (result != ChessEngine::GameResult::Playing) {
                if(board.IsFlipped())
                    board.Mirror();
                std::cout << board.Fen() << std::endl;
                if(result == ChessEngine::GameResult::WhiteWon){
                    std::cout << "White won" << std::endl;
                }else if(result == ChessEngine::GameResult::BlackWon){
                    std::cout << "Black won" << std::endl;
                }else{
                    std::cout << "Draw" << std::endl;
                }
                break;
            }

            ChessEngine::Move best_move = ChessEngine::GetBestMove(board, board.IsFlipped() ? 6 : 6);
            std::string notation = best_move.AlgebraicNotation(board.IsFlipped());
            std::cout << i << ". " << notation << std::endl;

            board.PlayMove(best_move);
            board.Mirror();
            i++;

            if( i > 30)
                break;
        }
    }

    ChessEngine::Timer::Print();
    return 0;
}
