#ifndef PSEUDOMOVES_H
#define PSEUDOMOVES_H

#include "Move.h"
#include "Board.h"

namespace ChessEngine::PseudoMoves {
    // Gets pseudo single / double pushes , captures and promotions. Does not include en passant.
    void GetPseudoPawnMoves(Bitboard pawns, Bitboard own, Bitboard enemy, MoveList &move_list);

    // Gets pseudo captures and quiet knight moves.
    void GetPseudoKnightMoves(Bitboard knights, Bitboard own, MoveList &move_list);

    // Gets capture and quiet pseudo king moves. Does not include castling moves.
    // If the king does not exist , king's tile is set to 0 so this can cause invalid behaviour.
    void GetPseudoKingMoves(BoardTile king, Bitboard own, MoveList &move_list);

    // Gets capture and quiet pseudo queen moves.
    void GetPseudoQueenMoves(Bitboard queens, Bitboard own, Bitboard enemy, MoveList &move_list);

    // Gets capture and quiet pseudo rook moves.
    void GetPseudoRookMoves(Bitboard rooks, Bitboard own, Bitboard enemy, MoveList &move_list);

    // Gets capture and quiet pseudo bishop moves.
    void GetPseudoBishopMoves(Bitboard bishops, Bitboard own, Bitboard enemy, MoveList &move_list);

    // Gets all the pseudo moves for every piece of [own].
    void GetPseudoMoves(Board::Representation representation, MoveList& move_list);
}

#endif
