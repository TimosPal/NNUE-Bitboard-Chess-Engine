#include "AttackTables.h"
#include "FenParser.h"
#include "NNUE.h"
#include "ZobristKey.h"
#include "UCI.h"

int main() {
    {
        PROFILE_SCOPE("Program");
        ChessEngine::AttackTables::InitMoveTables();
        ChessEngine::NNUE::InitModel("nn-62ef826d1a6d.nnue");
        ChessEngine::Zobrist::InitZobristKeysArrays();
        ChessEngine::UCI::MainLoop();
    }

    ChessEngine::Timer::Print();
    return 0;
}
