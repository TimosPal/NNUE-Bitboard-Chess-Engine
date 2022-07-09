#include "representation/AttackTables.h"
#include "miscellaneous/FenParser.h"
#include "search/NNUE.h"
#include "search/ZobristKey.h"
#include "miscellaneous/UCI.h"

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
