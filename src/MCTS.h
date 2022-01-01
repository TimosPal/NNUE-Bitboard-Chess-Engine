#ifndef MCTS_H
#define MCTS_H

#include "Board.h"
#include "Search.h"

namespace ChessEngine::MCTS{
    Move Search(const Board& state, int iterations, bool for_white);
}

#endif
