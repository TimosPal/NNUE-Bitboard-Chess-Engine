#ifndef NNUE_WRAPPER_H
#define NNUE_WRAPPER_H

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

    void InitModel(char* file_name);
    void InitInput(Board::Representation representation, bool is_flipped, int* pieces, int* squares);
    int Evaluate(const Board& board);

}

#endif
