#include <iostream>

#include "AttackTables.h"
#include "FenParser.h"

int main() {
    ChessEngine::AttackTables::InitMoveTables();

    //std::string fen = "k6K/8/8/8/2p5/5pp1/2P1P1P1/8 w - - 0 1";
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    ChessEngine::Board::BoardInfo info = {};
    IF_ERROR(!ChessEngine::ParseFenString(fen, info), "Invalid fen string.")
    ChessEngine::Board board(info);

    auto repr = std::get<ChessEngine::Board::Representation>(info);
    /*
    repr.Kings().Draw();
    repr.Knights().Draw();
    repr.Bishops().Draw();
    repr.Queens().Draw();
    repr.Pawns().Draw();
    repr.EnPassant().Draw();
    */

    auto moves = board.GetLegalMoves();
    for(auto move : moves){
        ChessEngine::Board temp =  board;
        std::cout << std::endl << "Starting" << std::endl << std::endl;
        temp.Draw();
        std::cout << std::endl << move << std::endl << std::endl;
        temp.PlayMove(move);
        temp.Draw();
    }

    int perft_result = ChessEngine::Board::Perft(3, board);
    std::cout << perft_result << std::endl;

    return 0;
}
