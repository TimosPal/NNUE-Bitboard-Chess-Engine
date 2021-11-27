#ifndef PSEUDOMOVES_H
#define PSEUDOMOVES_H

#include "Move.h"
#include "Board.h"

namespace ChessEngine::PseudoMoves {
    // Gets pseudo single / double pushes , captures and promotions. Does not include en passant.
    void GetPawnMoves(const Board::Representation& rep, MoveList& move_list);

    // Gets pseudo captures and quiet knight moves.
    void GetKnightMoves(const Board::Representation& rep, MoveList &move_list);
    void GetKnightCaptures(const Board::Representation& rep, MoveList& move_list);

    // Gets capture and quiet pseudo king moves. Does not include castling moves.
    // If the king does not exist , king's tile is set to 0 so this can cause invalid behaviour.
    void GetKingMoves(const Board::Representation& rep, Board::CastlingRights rights, MoveList &move_list);
    void GetKingCaptures(const Board::Representation& rep, MoveList &move_list);

    // Gets capture and quiet pseudo queen moves.
    void GetQueenMoves(const Board::Representation& rep, MoveList &move_list);
    void GetQueenCaptures(const Board::Representation& rep, MoveList& move_list);

    // Gets capture and quiet pseudo rook moves.
    void GetRookMoves(const Board::Representation& rep, MoveList &move_list);
    void GetRookCaptures(const Board::Representation& rep, MoveList& move_list);

    // Gets capture and quiet pseudo bishop moves.
    void GetBishopMoves(const Board::Representation& rep, MoveList &move_list);
    void GetBishopCaptures(const Board::Representation& rep, MoveList& move_list);

    // Gets all the pseudo moves for every piece of [own].
    void GetMoves(const Board::Representation& representation, Board::CastlingRights rights, MoveList& move_list);
    void GetCaptures(const Board::Representation& representation, MoveList& move_list);

}

#endif
