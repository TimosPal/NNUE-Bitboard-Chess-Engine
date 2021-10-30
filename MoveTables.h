#ifndef MOVETABLES_H
#define MOVETABLES_H

#include "Bitboard.h"

namespace ChessEngine::MoveTables{
    void InitMoveTables();

    Bitboard PawnsAttacks(uint8_t tile_index);
    Bitboard KnightAttacks(uint8_t tile_index);
    Bitboard KingAttacks(uint8_t tile_index);
    Bitboard RookAttacks(uint8_t tile_index, Bitboard occupancies);
    Bitboard BishopAttacks(uint8_t tile_index, Bitboard occupancies);
    Bitboard QueenAttacks(uint8_t tile_index, Bitboard occupancies);

}

#endif
