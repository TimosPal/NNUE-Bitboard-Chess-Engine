#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <tuple>

namespace ChessEngine {

    class BoardTile;

    class Bitboard {
    public:
        Bitboard(uint64_t value) : data_(value) {}
        Bitboard(BoardTile tile);
        Bitboard() = default;

        bool Get(uint8_t index) const;
        bool Get(uint8_t file, uint8_t rank) const;
        bool Get(BoardTile tile) const;

        void Set(uint8_t index);
        void Set(uint8_t file, uint8_t rank);
        void Set(BoardTile tile);
        void SetIf(BoardTile tile, bool cond);

        // Mirrors the board vertically.
        void Mirror();
        void Draw() const;

        friend Bitboard operator|(const Bitboard& a, const Bitboard& b) { return {a.data_ | b.data_}; }
        friend Bitboard operator&(const Bitboard& a, const Bitboard& b) { return {a.data_ & b.data_}; }
        friend Bitboard operator>>(const Bitboard& a, const Bitboard& b) { return {a.data_ >> b.data_}; }
        friend Bitboard operator<<(const Bitboard& a, const Bitboard& b) { return {a.data_ << b.data_}; }

    private:
        uint64_t data_ = 0;
    };

    class BoardTile{
    public:

        BoardTile(uint8_t index) : tile_index_(index) {}
        BoardTile(uint8_t file, uint8_t rank) : tile_index_(rank * 8 + file) {}
        BoardTile() = default;

        uint8_t GetIndex() const{ return tile_index_; }

        uint8_t GetRank(){ return tile_index_ / 8; }
        uint8_t GetFile(){ return tile_index_ % 8; }
        std::tuple<uint8_t, uint8_t> GetCoords() { return {GetFile(), GetRank()}; }

        // Mirrors the tile index vertically,
        // meaning the file stays the same.
        // We assume the board size is 8x8.
        void Mirror(){ tile_index_ ^= 0b111000; }

    private:
        uint8_t tile_index_ = 0;
    };

}

#endif
