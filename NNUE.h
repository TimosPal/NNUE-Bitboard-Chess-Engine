#ifndef NNUE_WRAPPER_H
#define NNUE_WRAPPER_H

#define REMOVED_SQUARE 64 // TODO: better way?

#include "Utilities.h"
#include "Board.h"
#include "nnue-probe/nnue.h"

// Wrapper header file for the NNUE probe.

namespace ChessEngine {

    // Converts engine's types to the required input of the NNUE probe lib.
    int GetSide(const Team& team);
    int GetSide(bool is_flipped);
    int GetPieceEncoding(const PieceType& type, const Team& team);
    int GetPieceEncoding(PieceInfo pieceInfo);
    int GetPieceEncoding(const PieceType& type, bool is_flipped);
    int GetSquare(const BoardTile& tile);
    int GetSquare(BoardTile tile, bool is_flipped);

    void InitModel(char* file_name);
    void InitInput(Board::Representation representation, bool is_flipped, int* pieces, int* squares);
    int Evaluate(const Board& board);
    int EvaluateIncremental(const Board& board);

    void InitAccumulator(int ply);
    DirtyPiece* GetDirtyPiece(int ply);
    void CopyToNextAccumulator(int ply);

}

#endif
