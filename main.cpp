#include <iostream>

#include "AttackTables.h"
#include "FenParser.h"
#include "NNUE.h"
#include "Search.h"
#include "ZobristKey.h"

int main() {
    {
        PROFILE_SCOPE("Program");
        ChessEngine::AttackTables::InitMoveTables();
        ChessEngine::InitModel("nn-62ef826d1a6d.nnue");
        ChessEngine::Zobrist::InitZobristKeysArrays();

        // Free bishop capture with pawn.
        //std::string fen = "rn1qkbnr/ppp1pppp/8/3p1b2/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1";
        // Discovered attack , free queen tactic.
        //std::string fen = "4k3/5q1p/8/8/4B3/4R3/8/7K w - - 0 1";
        // Knight gambit "fake queen blunder".
        //std::string fen = "rnb1k2r/ppp1qppp/2p2n2/2b3B1/4P3/3P4/PPP3PP/RN1QKB1R b KQkq - 0 1";
        // Knight fork.
        //std::string fen = "rn3rk1/ppq3pp/2p2p1B/3p4/B2P1P2/4n1QP/PPP3P1/RN4K1 b - - 3 18";
        // Knight fork but inverted colors.
        //std::string fen = "rn4k1/ppp3p1/4N1qp/b2p1p2/3P4/2P2P1b/PPQ3PP/RN3RK1 w - - 0 1";
        // Simple capture.
        //std::string fen = "8/8/8/8/4qK2/8/1k6/8 w - - 0 1";
        // Check mate in 1 (w).
        //std::string fen = "3k4/7Q/3K4/8/8/8/8/8 w - - 0 1";
        // Check mate in 1 (b).
        //std::string fen = "3K4/7q/3k4/8/8/8/8/8 b - - 0 1";

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        // discovered attack free queen , more complex board. Does not find it.
        //std::string fen = "r1b2rk1/pp2qppp/3p4/2p5/4B1n1/2P2N2/P4PPP/RN1QR1K1 w - - 0 14";

        // Default starting position.
        std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

        // mid game.
        //std::string fen = "8/3r4/1p4kp/p2p2pN/P2R2P1/4P3/1P1n2PK/8 w - - 2 35";

        ChessEngine::Board::BoardInfo info = {};
        IF_ERROR(!ChessEngine::ParseFenString(fen, info), "Invalid fen string.")
        ChessEngine::Board board(info);

        board.Draw();

        std::cout << "Board eval : " << ChessEngine::Evaluate(board) << std::endl;
        std::cout << "Fen eval : " << nnue_evaluate_fen(fen.c_str()) << std::endl;
        std::cout << "Fen string" << std::endl;
        std::cout << board.Fen() << std::endl;

        //std::cout << ChessEngine::Perft(board, 5) << std::endl;
        //std::cout << GetBestMove(board, 6) << std::endl;

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

            ChessEngine::Move best_move = ChessEngine::GetBestMove(board, board.IsFlipped() ? 5 : 5);
            auto aa = ChessEngine::BoardTile(best_move.GetFrom());
            auto bb = ChessEngine::BoardTile(best_move.GetTo());
            if(board.IsFlipped()){
                aa.Mirror();
                bb.Mirror();
            }

            std::string s1;
            std::string s2;
            std::string s3 = "";
            ChessEngine::CoordsToNotation(aa.GetCoords(), s1);
            ChessEngine::CoordsToNotation(bb.GetCoords(), s2);
            ChessEngine::PieceType promotion = best_move.GetPromotion();
            if(promotion != ChessEngine::None){
                s3 = std::string(1, ChessEngine::PieceInfoToChar({promotion, ChessEngine::White}));
            }
            std::cout << i << ". " << s1 << s2 << s3 << std::endl;

            board.PlayMove(best_move);
            board.Mirror();
            i++;

            if( i > 1000)
                break;
        }

        //std::cout << board.GetZobristKey() << std::endl;

    }

    ChessEngine::Timer::Print();
    return 0;
}
