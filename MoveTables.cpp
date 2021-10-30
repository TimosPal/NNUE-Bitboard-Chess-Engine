#include "MoveTables.h"

#include "Utilities.h"
#include "MagicNumbers.h"

namespace ChessEngine::MoveTables{

    // Performance isn't crucial for these functions
    // since they are run once when producing the tables.
    // The board is mirrored so we only care about white.

    static Bitboard pawn_attacks[64];
    static Bitboard king_attacks[64];
    static Bitboard knight_attacks[64];

    // Does not include outer tiles.
    static Bitboard bishop_rays[64];
    static Bitboard rook_rays[64];

    // When overflowing / under flowing in files A,H we may end up in the
    // opposite direction producing faulty moves. The inverted fileMasks
    // handle those 2 cases.
    static Bitboard GetPawnAttacks(Bitboard board) {
        Bitboard left_attack, right_attack;
        left_attack = Bitboard(board).ShiftTowards({-1, 1}) & Masks::not_file_H;
        right_attack = Bitboard(board).ShiftTowards({1, 1}) & Masks::not_file_A;
        return left_attack | right_attack;
    }

    // Generate the push moves of pawns
    static Bitboard GetSinglePawnPushes(Bitboard board) {
        return Bitboard(board).ShiftTowards({0, 1});
    }

    // Generate the double push moves of pawns , check for occupancy only on the first move.
    static Bitboard GetDoublePawnPushes(Bitboard board, Bitboard occupancies) {
        Bitboard pushes = Bitboard(board & Masks::rank_2).ShiftTowards({0, 1}) - occupancies;
        return Bitboard(board).ShiftTowards({0, 1});
    }

    // Generate the attack moves of all pawns at the given bitboard based on color
    static Bitboard GetKnightAttacks(Bitboard board) {
        Bitboard moves{};

        moves |= Bitboard(board).ShiftTowards({-1, +2}) & Masks::not_file_H; // Up left.
        moves |= Bitboard(board).ShiftTowards({+1, +2}) & Masks::not_file_A; // Up right.
        moves |= Bitboard(board).ShiftTowards({-2, +1}) & Masks::not_file_GH; // Left up.
        moves |= Bitboard(board).ShiftTowards({+2, +1}) & Masks::not_file_AB; // Right up.

        moves |= Bitboard(board).ShiftTowards({-1, -2}) & Masks::not_file_H; // Down left.
        moves |= Bitboard(board).ShiftTowards({+1, -2}) & Masks::not_file_A; // Down right.
        moves |= Bitboard(board).ShiftTowards({-2, -1}) & Masks::not_file_GH; // Left down.
        moves |= Bitboard(board).ShiftTowards({+2, -1}) & Masks::not_file_AB; // Right down.

        return moves;
    }

    // Generate the attack moves of all pawns at the given bitboard based on color
    static Bitboard GetKingAttacks(Bitboard board) {
        Bitboard moves{};

        moves |= Bitboard(board).ShiftTowards({+0, +1}); // Up.
        moves |= Bitboard(board).ShiftTowards({+0, -1}); // Down.

        moves |= Bitboard(board).ShiftTowards({-1, +0}) & Masks::not_file_H; // Left.
        moves |= Bitboard(board).ShiftTowards({-1, +1}) & Masks::not_file_H; // Left Up.
        moves |= Bitboard(board).ShiftTowards({-1, -1}) & Masks::not_file_H; // Left Down.

        moves |= Bitboard(board).ShiftTowards({+1, +0}) & Masks::not_file_A; // Right.
        moves |= Bitboard(board).ShiftTowards({+1, +1}) & Masks::not_file_A; // Right Up.
        moves |= Bitboard(board).ShiftTowards({+1, -1}) & Masks::not_file_A; // Right Down.

        return moves;
    }

    // Generate the ray from a starting position towards a direction. Stops when it hits an occupancy bit.
    static Bitboard GetRay(BoardTile from, std::tuple<int8_t, int8_t> direction, Bitboard occupancies){
        auto [x, y] = from.GetCoords();
        auto [x_off, y_off] = direction;

        Bitboard ray{};
        while(x_off >= 0 && x_off < 8 && y_off >= 0 && y_off < 8){
            x += x_off;
            y += y_off;
            if(occupancies.Get(x, y))
                break;
            ray |= Bitboard({x, y});
        }

        return ray;
    }

    static Bitboard GetRookRays(BoardTile from, Bitboard occupancies = 0){
        return GetRay(from, {0,1}, occupancies) | GetRay(from, {0,-1}, occupancies) |
        GetRay(from, {1,0}, occupancies) | GetRay(from, {-1,0}, occupancies);
    }

    static Bitboard GetBishopRays(BoardTile from, Bitboard occupancies = 0){
        return GetRay(from, {1,1}, occupancies) | GetRay(from, {-1,1}, occupancies) |
               GetRay(from, {-1,-1}, occupancies) | GetRay(from, {1,-1}, occupancies);
    }

    void InitMoveTables(){
        for (uint8_t rank = 0; rank < 8; rank++) {
            for (uint8_t file = 0; file < 8; file++) {
                BoardTile tile = BoardTile(file, rank);
                uint8_t index = tile.GetIndex();

                pawn_attacks[index] = GetPawnAttacks(tile);
                king_attacks[index] = GetKingAttacks(tile);
                knight_attacks[index] = GetKnightAttacks(tile);

                bishop_rays[index] = GetBishopRays(tile, Masks::outer_tiles);
                rook_rays[index] = GetRookRays(tile, Masks::outer_tiles);
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
        Bitboard rays = rook_rays[tile_index];
        auto key = MagicNumbers::RookMagicHash(rays & occupancies, tile_index);
    }

    Bitboard BishopAttacks(uint8_t tile_index, Bitboard occupancies){
        Bitboard rays = bishop_rays[tile_index];
        auto key = MagicNumbers::BishopMagicHash(rays & occupancies, tile_index);
    }

    Bitboard QueenAttacks(uint8_t tile_index, Bitboard occupancies){
        return RookAttacks(tile_index, occupancies) | BishopAttacks(tile_index, occupancies);
    }

}
