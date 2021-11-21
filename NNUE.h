#ifndef NNUE_H
#define NNUE_H

#include "Utilities.h"
#include "Board.h"

// Wrapper header file for the NNUE probe.

namespace  ChessEngine{

    // Converts engine's types to the required input of the NNUE probe lib.

    int GetSide(const Team& team);
    int GetSide(bool is_flipped);
    int GetPieceEncoding(const PieceType& type, const Team& team);
    int GetPieceEncoding(const ChessEngine::PieceInfo& info);
    int GetSquare(const BoardTile& tile, bool is_flipped);

    void InitInput(const Board::Representation& representation , int* pieces, int* squares);
    int Evaluate(const Board::Representation& representation);

}

#endif
