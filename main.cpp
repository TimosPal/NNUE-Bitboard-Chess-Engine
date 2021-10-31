#include <iostream>

#include "AttackTables.h"
#include "FenParser.h"

int main() {
    ChessEngine::AttackTables::InitMoveTables();

    std::string fen = "8/5p1p/8/4p2p/p1p1K3/6p1/4p3/3p4 w - - 0 1";
    ChessEngine::Board::BoardInfo info = {};
    IF_ERROR(!ChessEngine::ParseFenString(fen, info), "Invalid fen string.")
    ChessEngine::Board board(info);

    return 0;
}
