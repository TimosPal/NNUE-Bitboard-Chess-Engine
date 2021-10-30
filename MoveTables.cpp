#include "MoveTables.h"

#include "Bitboard.h"
#include "Utilities.h"

namespace ChessEngine::MoveTables{

    // Performance isn't crucial for these functions
    // since they are run once when producing the tables.
    // The board is mirrored so we only care about white.

    static Bitboard pawn_attacks[64];
    static Bitboard king_moves[64];
    static Bitboard knight_moves[64];

    // When overflowing / under flowing in files A,H we may end up in the
    // opposite direction producing faulty moves. The inverted fileMasks
    // handle those 2 cases.
    static Bitboard GetPawnAttacks(Bitboard board) {
        Bitboard left_attack, right_attack;
        left_attack = Bitboard(board).ShiftTowards({-1, 1}) & Masks::not_file_H;
        right_attack = Bitboard(board).ShiftTowards({1, 1}) & Masks::not_file_A;
        return left_attack | right_attack;
    }

    /* Generate the push moves of pawns */
    static Bitboard GetSinglePawnPushes(Bitboard board) {
        return Bitboard(board).ShiftTowards({0, 1});
    }

    /* Generate the double push moves of pawns , check for occupancy only on the first move. */
    static Bitboard GetDoublePawnPushes(Bitboard board, Bitboard occupancies) {
        Bitboard pushes = Bitboard(board & Masks::r2).ShiftTowards({0, 1}) - occupancies;
        return Bitboard(board).ShiftTowards({0, 1});
    }

    /* Generate the attack moves of all pawns at the given bitboard based on color */
    static Bitboard GetKnightMoves(Bitboard board) {
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

    /* Generate the attack moves of all pawns at the given bitboard based on color */
    static Bitboard GetKingMoves(Bitboard board) {
        Bitboard moves{};

        moves |= Bitboard(board).ShiftTowards({+0, +1});
        moves |= Bitboard(board).ShiftTowards({+0, -1});

        moves |= Bitboard(board).ShiftTowards({-1, +0}) & Masks::not_file_H;
        moves |= Bitboard(board).ShiftTowards({-1, +1}) & Masks::not_file_H;
        moves |= Bitboard(board).ShiftTowards({-1, -1}) & Masks::not_file_A;

        moves |= Bitboard(board).ShiftTowards({+1, +0}) & Masks::not_file_A;
        moves |= Bitboard(board).ShiftTowards({+1, +1}) & Masks::not_file_A;
        moves |= Bitboard(board).ShiftTowards({+1, -1}) & Masks::not_file_H;

        return moves;
    }

    void InitMoveTables(){
        auto InitTable = [] (Bitboard* table, Bitboard GetMoves(Bitboard)) {
            for (uint8_t rank = 0; rank < 8; rank++) {
                for (uint8_t file = 0; file < 8; file++) {
                    BoardTile tile = BoardTile(file, rank);
                    uint8_t index = tile.GetIndex();
                    table[index] = GetMoves(Bitboard(tile));
                }
            }
        };

        InitTable(pawn_attacks, GetPawnAttacks);
        InitTable(king_moves, GetKingMoves);
        InitTable(knight_moves, GetKnightMoves);
    }


}
