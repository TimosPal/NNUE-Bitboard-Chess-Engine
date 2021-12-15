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
        ChessEngine::InitModel("nn-62ef826d1a6d.nnue");
        ChessEngine::Zobrist::InitZobristKeysArrays();
        //ChessEngine::UCI::MainLoop();

        std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

        ChessEngine::Board::BoardInfo info = {};
        IF_ERROR(!ChessEngine::ParseFenString(fen, info), "Invalid fen string.")
        ChessEngine::Board board(info);

        //std::cout << GetBestMove(board, 8) << std::endl;

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

            ChessEngine::Move move_from_not = ChessEngine::Move(notation, board.IsFlipped());
            std::cout << i << ". " << move_from_not.AlgebraicNotation(board.IsFlipped())  << std::endl;


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
