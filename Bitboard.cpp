#include "Bitboard.h"

#include <iostream>

#include "BoardTile.h"

namespace ChessEngine {

    namespace {
        uint8_t BitScanForward(uint64_t word) { // TODO: use built_in_ffs
            /* De Bruijn bitscan algorithm */
            const uint64_t debruijn64 = 0x03f79d71b4cb0a89;
            const uint8_t index64[64] = {
                    0, 1, 48, 2, 57, 49, 28, 3,
                    61, 58, 50, 42, 38, 29, 17, 4,
                    62, 55, 59, 36, 53, 51, 43, 22,
                    45, 39, 33, 30, 24, 18, 12, 5,
                    63, 47, 56, 27, 60, 41, 37, 16,
                    54, 35, 52, 21, 44, 32, 23, 11,
                    46, 26, 40, 15, 34, 20, 31, 10,
                    25, 14, 19, 9, 13, 8, 7, 6
            };


            return index64[((word & ~(word-1)) * debruijn64) >> 58];
        }
    }

    Bitboard::Bitboard(BoardTile tile) : data_(std::uint64_t(1) << tile.GetIndex()) {}

    void Bitboard::Mirror(){
        data_ = (data_ & 0x00000000FFFFFFFF) << 32 | (data_ & 0xFFFFFFFF00000000) >> 32;
        data_ = (data_ & 0x0000FFFF0000FFFF) << 16 | (data_ & 0xFFFF0000FFFF0000) >> 16;
        data_ = (data_ & 0x00FF00FF00FF00FF) << 8 | (data_ & 0xFF00FF00FF00FF00) >> 8;
    }

    void Bitboard::Draw() const{
        for (uint8_t rank = 0; rank < 8; rank++) {
            for (uint8_t file = 0; file < 8; file++) {
                bool has_bit = Get(file, 8 - rank - 1);
                char symbol = (has_bit) ? '1' : '-';
                std::cout << symbol << "  ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    Bitboard& Bitboard::ShiftTowards(std::tuple<int8_t, int8_t> direction) {
        // index_curr = y * 8 + x
        // index_new = (y+y_off)*8 + (x+x_off)
        // index_new - index_curr =  8*y_off + x_off.
        auto[x_offset, y_offset] = direction;
        int8_t offset = 8*y_offset + x_offset;
        if(offset > 0)
            data_ <<= offset;
        else
            data_ >>= abs(offset);
        return *this;
    }

    bool Bitboard::Get(uint8_t index) const{
        return data_ & (std::uint64_t(1) << index);
    }

    bool Bitboard::Get(BoardTile tile) const{
        return Get(tile.GetIndex());
    }

    bool Bitboard::Get(uint8_t file, uint8_t rank) const{
        return Get(BoardTile(file, rank));
    }

    void Bitboard::Set(uint8_t index){
        data_ |= std::uint64_t(1) << index;
    }

    void Bitboard::Set(BoardTile tile) {
        Set(tile.GetIndex());
    }

    void Bitboard::Set(uint8_t file, uint8_t rank){
        Set(BoardTile(file, rank));
    }

    void Bitboard::SetIf(BoardTile tile, bool cond) {
        data_ |= std::uint64_t(cond) << tile.GetIndex();
    }

    BoardTile Bitboard::Iterator::operator*() const {
        return BitScanForward(data_);
    }

}