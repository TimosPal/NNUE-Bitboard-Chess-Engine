#include "Bitboard.h"

namespace ChessEngine {

    Bitboard::Bitboard(BoardTile tile) : data_(std::uint64_t(1) << tile.GetIndex()) {}

    void Bitboard::Set(BoardTile tile) {
        data_ |= Bitboard(tile).data_;
    }

    void Bitboard::Set_If(BoardTile tile, bool cond) {
        data_ |= std::uint64_t(cond) << tile.GetIndex();
    }

}