#ifndef ZOBRISTKEY_H
#define ZOBRISTKEY_H

#include "../representation/Board.h"
#include "../miscellaneous/Utilities.h"

namespace ChessEngine::Zobrist {
    uint64_t GetPieceSquareKey(PieceType type, bool is_white, uint8_t tile_index);
    uint64_t GetEnPassantKey(uint8_t tile_file);
    uint64_t GetCastlingKey(Board::CastlingRights rights, bool is_flipped);
    uint64_t GetSideKey();

    void InitZobristKeysArrays();
    uint64_t GetZobristKey(Board board, bool is_flipped);
}

#endif
