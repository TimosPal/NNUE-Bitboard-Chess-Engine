#ifndef ZOBRISTKEY_H
#define ZOBRISTKEY_H

#include "Board.h"

namespace ChessEngine {
    uint64_t GetPieceSquareKey(PieceType type, uint8_t tile_index);
    uint64_t GetEnPassantKey(uint8_t tile_file);
    uint64_t GetCastlingKey(uint8_t castling);
    uint64_t GetSideKey();

    void InitRandomKeys();
    uint64_t ZobristKey(const Board& board, bool is_flipped);
}

#endif
