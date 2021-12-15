#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <tuple>

#include "Bitboard.h"
#include "BoardTile.h"
#include "Utilities.h"
#include "Move.h"

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
            Bitboard Queens() const { return rook_queens & bishop_queens; }
            Bitboard Bishops() const { return bishop_queens - rook_queens; }
            Bitboard Pawns() const { return pawns_enPassant - Masks::rank_1_8; }
            Bitboard EnPassant() const { return pawns_enPassant & Masks::rank_1_8; }
            Bitboard Kings() const { return own_king | enemy_king; }
            Bitboard Knights() const {
                return (own_pieces | enemy_pieces)
                       - rook_queens - bishop_queens - Pawns()
                       - own_king - enemy_king;
            }

            // Mirrors the representation vertically
            void Mirror();
        };

        struct MoveCounters{
            // 50 move rule counter.
            uint8_t half_moves = 0;
            // Game's plys.
            uint8_t full_moves = 0;
            uint8_t ply_counter = 0;
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

            uint8_t AsInt() const { return data_; }

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
        Board() = default;

        bool IsFlipped() const { return is_flipped_; }

        const Representation& GetRepresentation() const { return representation_; }
        const uint8_t GetPlyCounter() const { return move_counters_.ply_counter; }
        CastlingRights GetCastlingRights() const { return castling_rights_; }
        uint64_t GetZobristKey() const { return zobrist_key_; }

        MoveList GetLegalQuietMoves() const;
        MoveList GetLegalCaptures() const;

        void PlayMove(Move move); // Plays the move. Does not alter the turn.
        void PlayNullMove();
        void UnPlayMove(Move move, PieceType captured_piece);
        void Mirror(); // Mirrors the board vertically. Changes turn.
        GameResult Result(const MoveList& moves) const;

        // The following functions do not account for mirroring.
        void Draw() const;
        std::string Fen() const;
        PieceInfo GetPieceInfoAt(uint8_t file, uint8_t rank) const;
        PieceInfo GetPieceInfoAt(BoardTile tile) const;
        PieceType GetPieceTypeAt(uint8_t file, uint8_t rank) const;

        bool IsInCheck() const;
    private:

        Bitboard GetPins() const;
        bool IsLegalMove(const Move& move, const Bitboard& pins, bool is_in_check) const;
        bool IsUnderAttack(BoardTile tile) const;
        bool InsufficientMaterial() const;

        Representation representation_;
        CastlingRights castling_rights_;
        MoveCounters move_counters_;

        // default state corresponds to white.
        bool is_flipped_ = false;
        uint64_t zobrist_key_;
    };

}

#endif