#ifndef BOARDTILE_H
#define BOARDTILE_H

#include <cstdint>
#include <tuple>

#include "Bitboard.h"

namespace ChessEngine {

    class BoardTile {
    public:
        explicit BoardTile(uint8_t index) : tile_index_(index) {}
        BoardTile(uint8_t file, uint8_t rank) : tile_index_(rank * 8 + file) {}
        BoardTile() = default;

        uint8_t GetIndex() const { return tile_index_; }
        uint8_t GetRank() { return tile_index_ / 8; }
        uint8_t GetFile() { return tile_index_ % 8; }

        std::tuple<uint8_t, uint8_t> GetCoords() { return {GetFile(), GetRank()}; }

        // Mirrors the tile index vertically,
        // meaning the file stays the same.
        // We assume the board size is 8x8.
        void Mirror() { tile_index_ ^= 0b111000; }

        friend Bitboard operator|(const Bitboard &a, const BoardTile &b) { return a | Bitboard(b);}
        friend Bitboard operator|(const BoardTile &a, const BoardTile &b) { return Bitboard(a) | Bitboard(b); }
        friend Bitboard operator-(const Bitboard &a, const BoardTile &b) { return a - Bitboard(b); }

    private:
        uint8_t tile_index_ = 0;
    };

}

#endif