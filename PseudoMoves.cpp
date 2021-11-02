#include "PseudoMoves.h"

#include "AttackTables.h"

namespace ChessEngine::PseudoMoves {

    // For move generation we take advantage of the fact that white is always the playing side. For example pawns
    // only have to move upwards.

    namespace {
        // Used for basic movement of kings and knights. Does not work with pawns.
        template<typename GetAttacks>
        void GetPseudoLeaperMoves(Bitboard piece, Bitboard own, MoveList& move_list, GetAttacks get) {
            for(auto from : piece){
                Bitboard attacks = get(from.GetIndex()) - own;
                for(auto to : attacks){
                    Move move = Move(from, to, PieceType::None);
                    move_list.push_back(move);
                }
            }
        }

        // Used for basic movement of slider pieces.
        template<typename GetAttacks>
        void GetSliderPseudoMoves(Bitboard piece, Bitboard own, Bitboard enemy, MoveList& move_list, GetAttacks get) {
            for(auto from : piece){
                Bitboard all = own | enemy;
                Bitboard attacks = get(from.GetIndex(), all) - own;
                for(auto to : attacks){
                    Move move = Move(from.GetIndex(), to.GetIndex(), PieceType::None);
                    move_list.push_back(move);
                }
            }
        }
    }

    void GetPseudoPawnMoves(Bitboard pawns, Bitboard own, Bitboard enemy, Bitboard enPassant, MoveList& move_list) {
        // Since we can find the [from] value from each [to] , we calculate all the final moves in parallel
        // with shifts instead of using pre computed attack tables. This function avoids branches as much
        // as possible hence the very exhaustive implementations of each case in separate loops.

        // TODO : simpler shifts for these cases to avoid branches.
        auto process_promotions = [](Bitboard moves, uint8_t from_offset, MoveList& move_list){
            for(auto to : moves){
                BoardTile from = to + from_offset;
                move_list.push_back(Move(from, to, PieceType::Rook));
                move_list.push_back(Move(from, to, PieceType::Bishop));
                move_list.push_back(Move(from, to, PieceType::Queen));
                move_list.push_back(Move(from, to, PieceType::Knight));
            }
        };

        auto process_captures_quiet = [](Bitboard moves, uint8_t from_offset, MoveList& move_list){
            for(auto to : moves){
                BoardTile from = BoardTile(to.GetIndex() + from_offset);
                Move move = Move(from, to, PieceType::None);
                move_list.push_back(move);
            }

        };

        constexpr int8_t one_back_offset = -8;
        constexpr int8_t one_right_offset = 1;
        constexpr int8_t one_left_offset = -1;

        // En passant is in ranks 1 and 8. Since each time the board is mirrored we only
        // need to check rank 8. We can consider said spot an enemy piece. This does not
        // affect forward pushes because in front of en passant tile there is an enemy piece.
        enemy |= enPassant.ShiftTowards({0,-2});

        Bitboard all = own | enemy;
        Bitboard pawns_up = pawns.ShiftTowards({0, 1}) - all;
        Bitboard promotions = pawns_up & Masks::rank_8;
        Bitboard quiet = pawns_up - promotions;

        // Quiet single push.
        process_captures_quiet(quiet, one_back_offset, move_list);
        // Push promotions.
        process_promotions(promotions, one_back_offset, move_list);
        // Double push.
        pawns_up = (pawns_up & Masks::rank_3).ShiftTowards({0, 1}) - all;
        process_captures_quiet(pawns_up, 2 * one_back_offset, move_list);

        Bitboard captures_left = pawns.ShiftTowards({-1,1}) & enemy & Masks::not_file_H;
        Bitboard captures_right = pawns.ShiftTowards({1,1}) & enemy & Masks::not_file_A;
        Bitboard capture_promotion_left = captures_left & Masks::rank_8;
        Bitboard capture_promotion_right = captures_right & Masks::rank_8;
        Bitboard capture_simple_left = captures_left - capture_promotion_left;
        Bitboard capture_simple_right = captures_right - capture_promotion_right;

        // Capture simple left.
        process_captures_quiet(capture_simple_left, one_back_offset + one_right_offset, move_list);
        // Capture simple right.
        process_captures_quiet(capture_simple_right, one_back_offset + one_left_offset, move_list);
        // Capture promotion left.
        process_promotions(capture_promotion_left, one_back_offset + one_right_offset, move_list);
        // Capture promotion right.
        process_promotions(capture_promotion_right, one_back_offset + one_left_offset, move_list);

    }

    void GetPseudoKnightMoves(Bitboard knights, Bitboard own, MoveList& move_list) {
        GetPseudoLeaperMoves(knights, own, move_list, AttackTables::KnightAttacks);
    }

    void GetPseudoKingMoves(BoardTile king, Bitboard own, Bitboard enemy, Bitboard rooks, MoveList& move_list) {
        GetPseudoLeaperMoves(Bitboard(king), own, move_list, AttackTables::KingAttacks);

        // Castling. King should be at the default position.
        // The castling flags will be checked later.
        if(king != Masks::king_default)
            return;

        // Check if rooks exist at correct tiles and if in between tiles are empty.
        Bitboard all = own | enemy;
        BoardTile from = Masks::king_default;
        if(rooks.Get(Masks::queen_rook) && (all & Masks::queen_castling_tiles).IsEmpty()){
            BoardTile to = Masks::queen_rook + 1; // Right of rook.
            move_list.push_back(Move(from, to, PieceType::None));
        }
        if(rooks.Get(Masks::king_rook) && (all & Masks::king_castling_tiles).IsEmpty()){
            BoardTile to = Masks::king_rook - 1; // Left of rook.
            move_list.push_back(Move(from, to, PieceType::None));
        }
    }

    void GetPseudoQueenMoves(Bitboard queens, Bitboard own, Bitboard enemy, MoveList& move_list) {
        GetSliderPseudoMoves(queens, own, enemy, move_list, AttackTables::QueenAttacks);
    }

    void GetPseudoRookMoves(Bitboard rooks, Bitboard own, Bitboard enemy, MoveList& move_list) {
        GetSliderPseudoMoves(rooks, own, enemy, move_list, AttackTables::RookAttacks);
    }

    void GetPseudoBishopMoves(Bitboard bishops, Bitboard own, Bitboard enemy, MoveList& move_list) {
        GetSliderPseudoMoves(bishops, own, enemy, move_list, AttackTables::BishopAttacks);
    }

    void GetPseudoMoves(Board::Representation representation, MoveList& move_list){
        Bitboard own = representation.own_pieces;
        Bitboard enemy = representation.enemy_pieces;
        Bitboard enPassant = representation.EnPassant();

        GetPseudoKnightMoves(representation.Knights() & own, own, move_list);
        GetPseudoKingMoves(representation.own_king, own, enemy, representation.Rooks(), move_list);
        GetPseudoRookMoves(representation.Rooks() & own, own, enemy, move_list);
        GetPseudoBishopMoves(representation.Bishops() & own, own, enemy, move_list);
        GetPseudoQueenMoves(representation.Queens() & own, own, enemy, move_list);
        GetPseudoPawnMoves(representation.Pawns() & own, own, enemy, enPassant, move_list);
    }

}