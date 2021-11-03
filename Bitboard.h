#ifndef BITBOARD_H
#define BITBOARD_H

#include <cstdint>
#include <tuple>

namespace ChessEngine {

    class BoardTile;

    class Bitboard {
    public:
        class Iterator{
        public:
            explicit Iterator(uint64_t data): data_(data) {}
            Iterator& operator++() { data_ &= (data_ - 1); return *this; } // Remove lsb.
            BoardTile operator*() const; // Gets LSB tile.

            friend bool operator!= (const Iterator& a, const Iterator& b) { return a.data_ != b.data_; };
        private:

            uint64_t data_;
        };

        explicit Bitboard(uint64_t value) : data_(value) {}
        explicit Bitboard(uint8_t file, uint8_t rank);
        explicit Bitboard(BoardTile tile);
        Bitboard() = default;

        bool Get(uint8_t index) const;
        bool Get(uint8_t file, uint8_t rank) const;
        bool Get(BoardTile tile) const;

        void Set(uint8_t index);
        void Set(uint8_t file, uint8_t rank);
        void Set(BoardTile tile);
        void SetIf(BoardTile tile, bool cond);

        void Reset(uint8_t index);
        void Reset(uint8_t file, uint8_t rank);
        void Reset(BoardTile tile);

        uint64_t AsInt() const { return data_; }

        Bitboard ShiftTowards(std::tuple<int8_t, int8_t> direction) const;

        // Mirrors the board vertically.
        void Mirror();
        void Draw() const;
        bool IsEmpty() const { return data_ == 0; }

        Iterator begin() const { return Iterator(data_); }
        Iterator end() const { return Iterator(0); }

        friend Bitboard operator|(const Bitboard& a, const Bitboard& b) { return Bitboard(a.data_ | b.data_); }
        friend Bitboard operator&(const Bitboard& a, const Bitboard& b) { return Bitboard(a.data_ & b.data_); }
        friend Bitboard operator>>(const Bitboard& a, const Bitboard& b) { return Bitboard(a.data_ >> b.data_); }
        friend Bitboard operator<<(const Bitboard& a, const Bitboard& b) { return Bitboard(a.data_ << b.data_); }
        friend Bitboard operator-(const Bitboard& a, const Bitboard& b) { return Bitboard(a.data_ & ~b.data_); }
        friend Bitboard operator~(const Bitboard& a) { return Bitboard(~a.data_); }

        Bitboard& operator&=(const Bitboard& a) { data_ &= a.data_; return *this; }
        Bitboard& operator|=(const Bitboard& a) { data_ |= a.data_; return *this; }
        Bitboard& operator-=(const Bitboard& a) { data_ &= ~a.data_; return *this; }

        bool operator==(const Bitboard& other) const { return data_ == other.data_; }
        bool operator!=(const Bitboard& other) const { return data_ != other.data_; }

    private:
        uint64_t data_ = 0;
    };

}

#endif
