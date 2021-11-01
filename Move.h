#ifndef MOVE_H
#define MOVE_H

#include <vector>
#include <stdint.h>

#include "Utilities.h"

namespace ChessEngine {

    class Move {
    public:

        Move(uint8_t from, uint8_t to, PieceType promotion):
        data_((from & Masks::From) | ((to << 6) & Masks::To) | ((promotion << 12) & Masks::Promotion))
        {}
        Move(BoardTile from, BoardTile to, PieceType promotion) : Move(from.GetIndex(), to.GetIndex(), promotion) {}

        uint8_t GetFrom() const {return data_ & Masks::From; }
        uint8_t GetTo() const {return (data_ & Masks::From) >> 6; }
        PieceType GetPromotion() const {return static_cast<PieceType>((data_ & Masks::From) >> 12); }

    private:
        enum Masks{
            From = 0b111111,
            To = 0b111111 << 6,
            Promotion = 0b1111 << 12
        };

        // 6 bits : from
        // 6 bits : to
        // 4 bits : promotion
        // Promotions are exactly 2^3 = 7.
        uint16_t data_;
    };

    using MoveList = std::vector<Move>;

}

#endif