#include "AttackTables.h"

#include "Utilities.h"
#include "MagicNumbers.h"

// TODO: _pex_u64 (not supported by my computer).

namespace ChessEngine::AttackTables{

    // Performance isn't crucial for these functions
    // since they are run once when producing the tables.
    // The board is mirrored so we only care about white.

    namespace {
        Bitboard pawn_attacks[64];
        Bitboard king_attacks[64];
        Bitboard knight_attacks[64];

        // Does not include outer tiles.
        Bitboard bishop_relevant_rays[64];
        Bitboard rook_relevant_rays[64];

        Bitboard sliding_pieces_attacks[MagicNumbers::permutations] = {};

        // When overflowing / under flowing in files A,H we may end up in the
        // opposite direction producing faulty moves. The inverted fileMasks
        // handle those 2 cases.
        Bitboard GetPawnAttacks(Bitboard board) {
            Bitboard left_attack, right_attack;
            left_attack = board.ShiftTowards({-1, 1}) & Masks::not_file_H;
            right_attack = board.ShiftTowards({1, 1}) & Masks::not_file_A;
            return left_attack | right_attack;
        }

        // Generate the attack moves of all pawns at the given bitboard based on color
        Bitboard GetKnightAttacks(Bitboard board) {
            Bitboard moves{};

            moves |= board.ShiftTowards({-1, +2}) & Masks::not_file_H; // Up left.
            moves |= board.ShiftTowards({+1, +2}) & Masks::not_file_A; // Up right.
            moves |= board.ShiftTowards({-2, +1}) & Masks::not_file_GH; // Left up.
            moves |= board.ShiftTowards({+2, +1}) & Masks::not_file_AB; // Right up.

            moves |= board.ShiftTowards({-1, -2}) & Masks::not_file_H; // Down left.
            moves |= board.ShiftTowards({+1, -2}) & Masks::not_file_A; // Down right.
            moves |= board.ShiftTowards({-2, -1}) & Masks::not_file_GH; // Left down.
            moves |= board.ShiftTowards({+2, -1}) & Masks::not_file_AB; // Right down.

            return moves;
        }

        // Generate the attack moves of all pawns at the given bitboard based on color
        Bitboard GetKingAttacks(Bitboard board) {
            Bitboard moves{};

            moves |= board.ShiftTowards({+0, +1}); // Up.
            moves |= board.ShiftTowards({+0, -1}); // Down.

            moves |= board.ShiftTowards({-1, +0}) & Masks::not_file_H; // Left.
            moves |= board.ShiftTowards({-1, +1}) & Masks::not_file_H; // Left Up.
            moves |= board.ShiftTowards({-1, -1}) & Masks::not_file_H; // Left Down.

            moves |= board.ShiftTowards({+1, +0}) & Masks::not_file_A; // Right.
            moves |= board.ShiftTowards({+1, +1}) & Masks::not_file_A; // Right Up.
            moves |= board.ShiftTowards({+1, -1}) & Masks::not_file_A; // Right Down.

            return moves;
        }

        // Generate the ray from a starting position towards a direction. Stops when it hits an occupancy bit.
        // Last tile is also included.
        Bitboard GetRay(BoardTile from, std::tuple<int8_t, int8_t> direction, Bitboard occupancies) {
            auto[x, y] = from.GetCoords();
            auto[x_off, y_off] = direction;

            x += x_off;
            y += y_off;
            Bitboard ray{};
            while (x >= 0 && x < 8 && y >= 0 && y < 8) {
                ray |= Bitboard(x, y);
                if (occupancies.Get(x, y))
                    break;
                x += x_off;
                y += y_off;
            }

            return ray;
        }

        Bitboard GetRookRays(BoardTile from, Bitboard occupancies) {
            return GetRay(from, {0, 1}, occupancies) | GetRay(from, {0, -1}, occupancies) |
                   GetRay(from, {1, 0}, occupancies) | GetRay(from, {-1, 0}, occupancies);
        }

        Bitboard GetBishopRays(BoardTile from, Bitboard occupancies) {
            return GetRay(from, {1, 1}, occupancies) | GetRay(from, {-1, 1}, occupancies) |
                   GetRay(from, {-1, -1}, occupancies) | GetRay(from, {1, -1}, occupancies);
        }

        template <typename Set>
        void ProduceSubSets(Bitboard set, Set process) {
            uint64_t occupants_permutation = 0;
            do {
                process(Bitboard(occupants_permutation));
                occupants_permutation = (occupants_permutation - set.AsInt()) & set.AsInt();
            } while (occupants_permutation != 0);
        }
    }

    void InitMoveTables(){
        for (uint8_t rank = 0; rank < 8; rank++) {
            for (uint8_t file = 0; file < 8; file++) {
                BoardTile tile = BoardTile(file, rank);
                Bitboard tile_board = Bitboard(tile);
                uint8_t index = tile.GetIndex();

                // Leaper pieces.
                pawn_attacks[index] = GetPawnAttacks(tile_board);
                king_attacks[index] = GetKingAttacks(tile_board);
                knight_attacks[index] = GetKnightAttacks(tile_board);

                // Relevant slider pieces rays. Meaning the ray in an empty board ,excluding outer edges.
                // If tile is an outer edge we have to stop at the corners and include the outer rank / file.
                // This is true only for rooks.
                bishop_relevant_rays[index] = GetBishopRays(tile, Masks::outer_tiles) - Masks::outer_tiles;
                bool is_at_edge = Masks::outer_tiles.Get(tile);
                Bitboard halting_mask = is_at_edge ? Masks::corner_tiles : Masks::outer_tiles;
                rook_relevant_rays[index] = GetRookRays(tile, halting_mask) - halting_mask;

                auto set_rooks = [=](Bitboard permutation){
                    auto key = MagicNumbers::RookMagicHash(permutation, index);
                    auto value = GetRookRays(tile, permutation);
                    assert(sliding_pieces_attacks[key] == Bitboard() || sliding_pieces_attacks[index] == value);
                    sliding_pieces_attacks[key] = value;
                };
                auto set_bishops = [=](Bitboard permutation){
                    auto key = MagicNumbers::BishopMagicHash(permutation, index);
                    auto value = GetBishopRays(tile, permutation);
                    assert(sliding_pieces_attacks[key] == Bitboard() || sliding_pieces_attacks[index] == value);
                    sliding_pieces_attacks[key] = value;
                };
                // Populates slider pieces attack table based on magic numbers for every permutation.
                ProduceSubSets(rook_relevant_rays[index], set_rooks);
                ProduceSubSets(bishop_relevant_rays[index], set_bishops);
            }
        }
    }

    Bitboard PawnsAttacks(uint8_t tile_index){
        return pawn_attacks[tile_index];
    }

    Bitboard KnightAttacks(uint8_t tile_index){
        return knight_attacks[tile_index];
    }

    Bitboard KingAttacks(uint8_t tile_index){
        return king_attacks[tile_index];
    }

    Bitboard RookAttacks(uint8_t tile_index, Bitboard occupancies){
        Bitboard rays = rook_relevant_rays[tile_index];
        auto key = MagicNumbers::RookMagicHash(rays & occupancies, tile_index);
        return sliding_pieces_attacks[key];
    }

    Bitboard BishopAttacks(uint8_t tile_index, Bitboard occupancies){
        Bitboard rays = bishop_relevant_rays[tile_index];
        auto key = MagicNumbers::BishopMagicHash(rays & occupancies, tile_index);
        return sliding_pieces_attacks[key];
    }

    Bitboard QueenAttacks(uint8_t tile_index, Bitboard occupancies){
        return RookAttacks(tile_index, occupancies) | BishopAttacks(tile_index, occupancies);
    }

}
