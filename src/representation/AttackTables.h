#ifndef ATTACKTABLES_H
#define ATTACKTABLES_H

#include <representation/Bitboard.h>

namespace ChessEngine::AttackTables{
    void InitMoveTables();

    Bitboard PawnsAttacks(uint8_t tile_index);
    Bitboard KnightAttacks(uint8_t tile_index);
    Bitboard KingAttacks(uint8_t tile_index);
    Bitboard RookAttacks(uint8_t tile_index, Bitboard occupancies = Bitboard());
    Bitboard BishopAttacks(uint8_t tile_index, Bitboard occupancies = Bitboard());
    Bitboard QueenAttacks(uint8_t tile_index, Bitboard occupancies = Bitboard());

}

#endif
