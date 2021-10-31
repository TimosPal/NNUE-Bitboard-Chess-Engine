#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <tuple>

#include "Bitboard.h"
#include "BoardTile.h"
#include "Utilities.h"

namespace ChessEngine {

    class Board {
    public:
        struct Representation{
            // Queens share the bitboards of rooks and bishops
            // Pawns can include the en passant square on ranks 1-8.
            Bitboard own_pieces = Bitboard(0);
            Bitboard enemy_pieces = Bitboard(0);
            Bitboard rook_queens = Bitboard(0);
            Bitboard bishop_queens = Bitboard(0);
            Bitboard pawns_enPassant = Bitboard(0);
            BoardTile own_king = BoardTile(0);
            BoardTile enemy_king = BoardTile(0);

            Bitboard Rooks() const { return rook_queens - bishop_queens; }
            Bitboard Bishops() const { return bishop_queens - rook_queens; }
            Bitboard Pawns() const { return pawns_enPassant - Masks::rank_1_8; }
            Bitboard EnPassant() const { return pawns_enPassant & Masks::rank_1_8; }
            Bitboard Kings() const { return own_king | enemy_king; }
            Bitboard Knights() const {
                return (own_pieces | enemy_pieces)
                       - rook_queens - bishop_queens - pawns_enPassant
                       - own_king - enemy_king;
            }

            // Mirrors the representation vertically
            void Mirror();

        };

        struct MoveCounters{
            uint8_t half_moves = 0;
            uint8_t full_moves = 0;
        };

        class CastlingRights{
        public:
            // Assuming bool is 0b1 we set the appropriate bits via shifts..
            CastlingRights(bool own_queen_side, bool own_king_side, bool enemy_queen_side, bool enemy_king_side) :
                    data_(own_queen_side | own_king_side << 1 | enemy_queen_side << 2 | enemy_king_side << 3) {}
            CastlingRights() = default;

            bool CanOwnQueenSide() const { return data_ & 1; }
            bool CanOwnKingSide() const { return data_ & 2; }
            bool CanEnemyQueenSide() const { return data_ & 4; }
            bool CanEnemyKingSide() const { return data_ & 8; }

            void ResetOwnQueenSide() { data_ &= ~1; }
            void ResetOwnKingSide() { data_ &= ~2; }
            void ResetEnemyQueenSide() { data_ &= ~4; }
            void ResetEnemyKingSide() { data_ &= ~8; }

            // Swaps the castling rights between own and enemy.
            // We dont use bitfields so we can swap the bits ourselves.
            void Mirror() { data_ = ((data_ & 0b11) << 2) + ((data_ & 0b1100) >> 2); }

        private:
            // Bit 1 -> own queen.
            // Bit 2 -> own king.
            // Bit 3 -> enemy queen.
            // Bit 4 -> enemy king.
            // By default all are disabled.
            uint8_t data_ = 0;
        };

        using BoardInfo = std::tuple<Representation, CastlingRights, MoveCounters, Team>;
        explicit Board(const BoardInfo &info);

        // Mirrors the board vertically.
        void Mirror();

    private:

        Representation representation_;
        CastlingRights castling_rights_;
        MoveCounters move_counters_;

        // default state corresponds to white.
        bool is_flipped_ = false;
    };

}

#endif
