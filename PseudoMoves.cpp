#include "PseudoMoves.h"

#include "AttackTables.h"

namespace ChessEngine::PseudoMoves {

    namespace {
        // Generate the push moves of pawns
        Bitboard GetSinglePawnPushes(Bitboard board) {
            return Bitboard(board).ShiftTowards({0, 1});
        }

        // Generate the double push moves of pawns , check for occupancy only on the first move.
        Bitboard GetDoublePawnPushes(Bitboard board, Bitboard occupancies) {
            Bitboard pushes = Bitboard(board & Masks::rank_2).ShiftTowards({0, 1}) - occupancies;
            return Bitboard(board).ShiftTowards({0, 1});
        }
    }

    // Used for basic movement of everything but pawns.
    void GetSimplePseudoMoves(Bitboard piece_board, Bitboard own_pieces, MoveList& move_list) {
        for(auto from : piece_board){
            Bitboard attacks = AttackTables::KnightAttacks(from.GetIndex()) - own_pieces;
            for(auto to : attacks){
                Move move = Move(from.GetIndex(), to.GetIndex(), PieceType::None);
                move_list.push_back(move);
            }
        }
    }

    void GetPawnPseudoMoves(Bitboard piece_board, Bitboard own_pieces, MoveList& move_list) {
        for(auto from : piece_board){
            Bitboard attacks = AttackTables::KnightAttacks(from.GetIndex()) - own_pieces;
            for(auto to : attacks){
                Move move = Move(from.GetIndex(), to.GetIndex(), PieceType::None);
                move_list.push_back(move);
            }
        }
    }

}