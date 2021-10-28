#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>

namespace ChessEngine {

    class BoardTile;

    class Bitboard {
    public:
        Bitboard(uint64_t value) : data_(value) {}
        Bitboard(BoardTile tile);
        Bitboard() = default;

        void Set(BoardTile tile);
        void Set_If(BoardTile tile, bool cond);

        // Mirrors the board vertically.
        void Mirror(){
            data_ = (data_ & 0x00000000FFFFFFFF) << 32 | (data_ & 0xFFFFFFFF00000000) >> 32;
            data_ = (data_ & 0x0000FFFF0000FFFF) << 16 | (data_ & 0xFFFF0000FFFF0000) >> 16;
            data_ = (data_ & 0x00FF00FF00FF00FF) << 8 | (data_ & 0xFF00FF00FF00FF00) >> 8;
        }

    private:
        uint64_t data_;
    };

    class BoardTile{
    public:
        BoardTile(uint8_t index) : tileIndex_(index) {}
        BoardTile(uint8_t file, uint8_t rank) : tileIndex_(rank * 8 + file) {}
        BoardTile() = default;

        uint8_t GetIndex() const{
            return tileIndex_;
        }

        // Mirrors the tile index vertically,
        // meaning the file stays the same.
        // We assume the board size is 8x8.
        void Mirror(){
            tileIndex_ ^= 0b111000;
        }

    private:
        uint8_t tileIndex_;
    };

}

#endif
