#include <iostream>

#include "AttackTables.h"
#include "FenParser.h"
#include "PseudoMoves.h"

int main() {
    ChessEngine::AttackTables::InitMoveTables();

    std::string fen = "5p2/4P3/8/8/8/8/8/5K1k w - - 0 1";
    ChessEngine::Board::BoardInfo info = {};
    IF_ERROR(!ChessEngine::ParseFenString(fen, info), "Invalid fen string.")
    ChessEngine::Board board(info);

    auto [a, b, c, d] = info;
    ChessEngine::MoveList moves;
    ChessEngine::PseudoMoves::GetPseudoMoves(a, moves);
    for (auto t : moves) {
        std::cout << t << std::endl;
    }

    return 0;
}
