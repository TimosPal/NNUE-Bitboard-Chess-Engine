#include <iostream>

#include "MoveTables.h"
#include "FenParser.h"

int main() {
    ChessEngine::MoveTables::InitMoveTables();

    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    ChessEngine::Board::BoardInfo info = {};
    IF_ERROR(!ChessEngine::ParseFenString(fen, info), "Invalid fen string.")
    ChessEngine::Board board(info);

    ChessEngine::MoveTables::RookAttacks(1,1);

    return 0;
}
