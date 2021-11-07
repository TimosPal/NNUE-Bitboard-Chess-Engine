#include <iostream>

#include "AttackTables.h"
#include "FenParser.h"

int main() {
    {
    PROFILE_SCOPE("Program");
    ChessEngine::AttackTables::InitMoveTables();

    std::string fen = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 1 0";
    ChessEngine::Board::BoardInfo info = {};
    IF_ERROR(!ChessEngine::ParseFenString(fen, info), "Invalid fen string.")
    ChessEngine::Board board(info);

    std::cout << board.Fen() << std::endl;

    int nodes = ChessEngine::Board::Perft(5, board);
    std::cout << "Nodes : " << nodes << std::endl;
    }
    ChessEngine::Timer::Print();

    return 0;
}
