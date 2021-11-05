#include <iostream>

#include "AttackTables.h"
#include "FenParser.h"

int main() {
    {
    PROFILE_SCOPE("Program");
    ChessEngine::AttackTables::InitMoveTables();

    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    ChessEngine::Board::BoardInfo info = {};
    IF_ERROR(!ChessEngine::ParseFenString(fen, info), "Invalid fen string.")
    ChessEngine::Board board(info);

    int nodes = ChessEngine::Board::Perft(5, board);
    std::cout << "Nodes : " << nodes << std::endl;
    }
    ChessEngine::Timer::Print();

    return 0;
}
