#include <iostream>

#include "AttackTables.h"
#include "FenParser.h"

int main() {
    ChessEngine::AttackTables::InitMoveTables();

    std::string fen = "k6K/8/8/2P5/3P1P1P/4P1P1/3P4/8 w - - 0 1";
    ChessEngine::Board::BoardInfo info = {};
    IF_ERROR(!ChessEngine::ParseFenString(fen, info), "Invalid fen string.")
    ChessEngine::Board board(info);

    for (int j = 0; j < 1000; j++) {
        ChessEngine::Board temp = board;
        for (int i = 0; i < 3000; i++) {
            auto moves = temp.GetLegalMoves();
            auto m = moves[rand() % moves.size()];
            temp.PlayMove(m);
        }
    }

    return 0;
}
