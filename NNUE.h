#ifndef NNUE_WRAPPER_H
#define NNUE_WRAPPER_H

#define REMOVED_SQUARE 64 // TODO: better way?
#define MAX_HSTACK 1024 // Max size of NNUE alligned memory.

#include "Utilities.h"
#include "Board.h"
#include "nnue-probe/nnue.h"

namespace ChessEngine {
    class NNUE{
    public:
        static NNUE& Instance() {
            static NNUE instance;
            return instance;
        }

        static void InitModel(char* file_name);

        static int Evaluate(const Board& board);
        int EvaluateIncremental(const Board& board);

        void InitAccumulator(int ply);
        void CopyToNextAccumulator(int ply);
        DirtyPiece* GetDirtyPiece(int ply);

        // Converts engine's types to the required input of the NNUE probe lib.
        static int GetSideEncoding(const Team& team);
        static int GetSideEncoding(bool is_flipped);
        static int GetPieceEncoding(const PieceType& type, const Team& team);
        static int GetPieceEncoding(PieceInfo pieceInfo);
        static int GetPieceEncoding(const PieceType& type, bool is_flipped);
        static int GetSquareEncoding(const BoardTile& tile);
        static int GetSquareEncoding(BoardTile tile, bool is_flipped);

    private:
        NNUE() { AlignedReserve<NNUEdata>(nnue_data_arr, MAX_HSTACK); }
        NNUEdata* nnue_data_arr;
    };

}

#endif
