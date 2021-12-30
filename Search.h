#ifndef SEARCH_H
#define SEARCH_H

#include "Board.h"

namespace ChessEngine {

    Move GetBestMove(const Board& board, int depth);
    int Perft(const Board& board, int depth);

}

#endif