#ifndef MOVE_H
#define MOVE_H

#include <vector>
#include <stdint.h>
#include <iostream>
#include <sstream>

#include "Utilities.h"

namespace ChessEngine {

    class Move {
    public:

        Move(uint8_t from, uint8_t to, PieceType promotion):
        data_((from & Masks::From) | ((to << 6) & Masks::To) | ((promotion << 12) & Masks::Promotion))
        {}
        Move(BoardTile from, BoardTile to, PieceType promotion) : Move(from.GetIndex(), to.GetIndex(), promotion) {}
        Move() = default;

        BoardTile GetFrom() const {return BoardTile(data_ & Masks::From); }
        BoardTile GetTo() const {return BoardTile((data_ & Masks::To) >> 6); }
        PieceType GetPromotion() const {return static_cast<PieceType>((data_ & Masks::Promotion) >> 12); }

        friend std::ostream& operator<<(std::ostream& os, const Move& move){
            std::string from_notation;
            CoordsToNotation(BoardTile(move.GetFrom()).GetCoords(), from_notation);
            std::string to_notation;
            CoordsToNotation(BoardTile(move.GetTo()).GetCoords(), to_notation);
            PieceInfo piece_info = {move.GetPromotion(), Team::White};
            os << from_notation << " -> " << to_notation << " | Promotion " << PieceInfoToChar(piece_info);
            return os;
        }

        std::string AlgebraicNotation(bool is_flipped);

        bool operator==(const Move& mv) const { return data_ == mv.data_; }
        bool operator!=(const Move& mv) const { return data_ != mv.data_; }

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
        uint16_t data_ = 0;
    };

    using MoveList = std::vector<Move>;

}

#endif