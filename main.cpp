#include <iostream>

#include "AttackTables.h"
#include "FenParser.h"
#include "NNUE.h"
#include "Search.h"
#include "ZobristKey.h"
#include "UCI.h"

#include "MCTS.h"

int main() {
    {
        PROFILE_SCOPE("Program");
        ChessEngine::AttackTables::InitMoveTables();
        ChessEngine::NNUE::InitModel("nn-62ef826d1a6d.nnue");
        ChessEngine::Zobrist::InitZobristKeysArrays();
        ChessEngine::UCI::MainLoop();
        return 0;

        // Kwstas d-7
        //std::string fen = "2r3k1/p2rqpp1/4b2p/4Q3/4B2P/1P2P1P1/P2R1R2/6K1 b - - 6 28";
        // Random game d-7
        //std::string fen = "r2qk2r/4pp1p/2p3Pb/1pB1P3/3PbP2/1pP5/P2QNKn1/R6R w kq - 0 21";
        // Default
        std::string fen = ChessEngine::starting_position_fen;

        //std::string fen = "8/5r2/6k1/7p/7P/6P1/3R1PK1/8 w - - 59 128";

        //std::string fen = "8/4K2k/8/8/6Q1/8/8/8 w - - 6 68";

        //std::string fen = "rn1q1k1r/1b2b1pp/p1p5/1p2N3/4PB2/5Q2/PP3PPP/R3K2R b KQ - 3 14";
        //std::string fen = "8/2Bk4/3P4/1P2K3/2P1P3/8/2r5/8 w - - 7 57";

        ChessEngine::Board::BoardInfo info = {};
        IF_ERROR(!ChessEngine::ParseFenString(fen, info), "Invalid fen string.")
        ChessEngine::Board board(info);

        //ChessEngine::Move mv = ChessEngine::MCTS::Search(board, 1000);
        //std::string notation = mv.AlgebraicNotation(board.IsFlipped());
        //std::cout << notation << std::endl;

        if(false) {
            int eval;
            auto best_move = GetBestMove(board, 8, eval);
            std::string notation = best_move.AlgebraicNotation(board.IsFlipped());
            std::cout << notation << std::endl;
        }

        int i = 1;
        while(true) {
            ChessEngine::Bitboard pins = board.GetPins();
            bool is_in_check = board.IsInCheck();
            auto moves = board.GetLegalCaptures(pins, is_in_check);
            auto quiet_moves = board.GetLegalQuietMoves(pins, is_in_check);
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

            ChessEngine::Move best_move;
            if(!board.IsFlipped()){
                best_move = ChessEngine::MCTS::Search(board, 1000, true);
            }else{
                int eval;
                best_move = ChessEngine::GetBestMove(board, 7, eval);
            }

            std::string notation = best_move.AlgebraicNotation(board.IsFlipped());
            std::cout << i << ". " << notation << std::endl;

            board.PlayMove(best_move);
            board.Mirror();
            i++;

            if(i > 2000)
                break;
        }
    }

    ChessEngine::Timer::Print();
    return 0;
}
