#ifndef SEARCH_H
#define SEARCH_H

#include "Board.h"

namespace ChessEngine {

    int NegaMax(const Board& board, int depth, int a, int b);
    Move GetBestMove(const Board& board, int depth);

}

#endif