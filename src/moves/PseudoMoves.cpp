#include "PseudoMoves.h"

#include <cassert>

#include "../representation/AttackTables.h"

namespace ChessEngine::PseudoMoves {

    // For move generation we take advantage of the fact that white is always the playing side. For example pawns
    // only have to move upwards.

    namespace {

        auto HandlePromotionAttacks(uint8_t from_offset, Bitboard moves, MoveList& move_list){
            for(auto to : moves){
                BoardTile from = to + from_offset;
                assert(from != to);
                move_list.push_back(Move(from, to, PieceType::Rook));
                move_list.push_back(Move(from, to, PieceType::Bishop));
                move_list.push_back(Move(from, to, PieceType::Queen));
                move_list.push_back(Move(from, to, PieceType::Knight));
            }
        };

        void HandleAttacks(uint8_t from_offset, Bitboard moves,  MoveList& move_list){
            for(auto to : moves){
                BoardTile from = BoardTile(to.GetIndex() + from_offset);
                assert(from != to);
                Move move = Move(from, to, PieceType::None);
                move_list.push_back(move);
            }
        }

        void HandleAttacks(BoardTile from, Bitboard moves, MoveList& move_list){
            for(auto to : moves){
                Move move = Move(from, to, PieceType::None);
                assert(from != to);
                move_list.push_back(move);
            }
        }

        // Used for basic movement of kings and knights. Does not work with pawns.
        template<typename GetAttacks>
        void GetLeaperMoves(Bitboard piece, Bitboard exclude, const Board::Representation& rep, MoveList& move_list, GetAttacks get) {
            for(auto from : piece){
                Bitboard attacks = get(from.GetIndex());
                HandleAttacks(from, attacks - exclude, move_list);
            }
        }

        // Used for basic movement of slider pieces.
        template<typename GetAttacks>
        void GetSliderMoves(Bitboard piece, Bitboard exclude, const Board::Representation& rep, MoveList& move_list, GetAttacks get) {
            Bitboard all = rep.own_pieces | rep.enemy_pieces;
            for(auto from : piece){
                Bitboard attacks = get(from.GetIndex(), all);
                HandleAttacks(from, attacks - exclude, move_list);
            }
        }
    }

    void GetPawnQuietMoves(const Board::Representation& rep, MoveList& move_list) {
        // Since we can find the [from] value from each [to] , we calculate all the final moves in parallel
        // with shifts instead of using pre computed attack tables. This function avoids branches as much
        // as possible hence the very exhaustive implementations of each case in separate loops.

        constexpr int8_t one_back_offset = -8;

        Bitboard enemy = rep.enemy_pieces;

        Bitboard pawns = rep.Pawns() & rep.own_pieces;;
        Bitboard all = rep.own_pieces | enemy;
        Bitboard pawns_up = pawns.ShiftUp1() - all;
        Bitboard promotions = pawns_up & Masks::rank_8;
        Bitboard quiet = pawns_up - promotions;

        // Quiet single push.
        HandleAttacks(one_back_offset, quiet, move_list);
        // Push promotions.
        HandlePromotionAttacks(one_back_offset, promotions, move_list);
        // Double push.
        Bitboard double_push = (pawns_up & Masks::rank_3).ShiftUp1() - all;
        HandleAttacks(2 * one_back_offset, double_push, move_list);
    }

    void GetPawnCaptures(const Board::Representation& rep, MoveList& move_list) {
        // Since we can find the [from] value from each [to] , we calculate all the final moves in parallel
        // with shifts instead of using pre computed attack tables. This function avoids branches as much
        // as possible hence the very exhaustive implementations of each case in separate loops.

        constexpr int8_t one_back_offset = -8;
        constexpr int8_t one_right_offset = 1;
        constexpr int8_t one_left_offset = -1;

        // En passant is in ranks 1 and 8. Since each time the board is mirrored we only
        // need to check rank 8. We can consider said spot an enemy piece. This does not
        // affect forward pushes because in front of en passant tile there is an enemy piece.
        Bitboard enemy = rep.enemy_pieces | rep.EnPassant().ShiftDown2();

        Bitboard pawns = rep.Pawns() & rep.own_pieces;

        Bitboard captures_left = pawns.ShiftUp1Left1() & enemy & Masks::not_file_H;
        Bitboard captures_right = pawns.ShiftUp1Right1() & enemy & Masks::not_file_A;
        Bitboard capture_promotion_left = captures_left & Masks::rank_8;
        Bitboard capture_promotion_right = captures_right & Masks::rank_8;
        Bitboard capture_simple_left = captures_left - capture_promotion_left;
        Bitboard capture_simple_right = captures_right - capture_promotion_right;

        // Capture simple left.
        HandleAttacks(one_back_offset + one_right_offset, capture_simple_left, move_list);
        // Capture simple right.
        HandleAttacks(one_back_offset + one_left_offset, capture_simple_right, move_list);
        // Capture promotion left.
        HandlePromotionAttacks(one_back_offset + one_right_offset, capture_promotion_left, move_list);
        // Capture promotion right.
        HandlePromotionAttacks(one_back_offset + one_left_offset, capture_promotion_right, move_list);
    }

    void GetKnightQuietMoves(const Board::Representation& rep, MoveList& move_list) {
        Bitboard knights = rep.Knights() & rep.own_pieces;
        GetLeaperMoves(knights, rep.own_pieces | rep.enemy_pieces, rep, move_list, AttackTables::KnightAttacks);
    }

    void GetKnightCaptures(const Board::Representation& rep, MoveList& move_list) {
        Bitboard knights = rep.Knights() & rep.own_pieces;
        GetLeaperMoves(knights, ~rep.enemy_pieces, rep, move_list, AttackTables::KnightAttacks);
    }

    void GetKingQuietMoves(const Board::Representation& rep, Board::CastlingRights rights, MoveList &move_list) {
        BoardTile king = rep.own_king;
        GetLeaperMoves(Bitboard(king), rep.own_pieces | rep.enemy_pieces, rep, move_list, AttackTables::KingAttacks);

        // Castling. King should be at the default position.
        // The castling flags will be checked later.
        if(king != Masks::king_default)
            return;

        // Check if rooks exist at correct tiles and if in between tiles are empty.
        Bitboard all = rep.own_pieces | rep.enemy_pieces;
        BoardTile from = Masks::king_default;
        if(rights.CanOwnQueenSide() && (all & Masks::queen_castling_tiles).IsEmpty()){
            BoardTile to = Masks::queen_rook + 2; // 2 tiles right from rook.
            assert(from != to);
            move_list.push_back(Move(from, to, PieceType::None));
        }
        if(rights.CanOwnKingSide() && (all & Masks::king_castling_tiles).IsEmpty()){
            BoardTile to = Masks::king_rook - 1; // Left of rook.
            assert(from != to);
            move_list.push_back(Move(from, to, PieceType::None));
        }
    }

    void GetKingCaptures(const Board::Representation& rep, MoveList &move_list) {
        BoardTile king = rep.own_king;
        GetLeaperMoves(Bitboard(king), ~rep.enemy_pieces, rep, move_list, AttackTables::KingAttacks);
    }

    void GetQueenQuietMoves(const Board::Representation& rep, MoveList& move_list) {
        Bitboard queens = rep.Queens() & rep.own_pieces;
        GetSliderMoves(queens, rep.own_pieces | rep.enemy_pieces, rep, move_list, AttackTables::QueenAttacks);
    }

    void GetQueenCaptures(const Board::Representation& rep, MoveList& move_list) {
        Bitboard queens = rep.Queens() & rep.own_pieces;
        GetSliderMoves(queens, ~rep.enemy_pieces, rep, move_list, AttackTables::QueenAttacks);
    }

    void GetRookQuietMoves(const Board::Representation& rep, MoveList& move_list) {
        Bitboard rooks = rep.Rooks() & rep.own_pieces;
        GetSliderMoves(rooks, rep.own_pieces | rep.enemy_pieces, rep, move_list, AttackTables::RookAttacks);
    }

    void GetRookCaptures(const Board::Representation& rep, MoveList& move_list) {
        Bitboard rooks = rep.Rooks() & rep.own_pieces;
        GetSliderMoves(rooks, ~rep.enemy_pieces, rep, move_list, AttackTables::RookAttacks);
    }

    void GetBishopQuietMoves(const Board::Representation& rep, MoveList& move_list) {
        Bitboard bishops = rep.Bishops() & rep.own_pieces;
        GetSliderMoves(bishops, rep.own_pieces | rep.enemy_pieces, rep, move_list, AttackTables::BishopAttacks);
    }

    void GetBishopCaptures(const Board::Representation& rep, MoveList& move_list) {
        Bitboard bishops = rep.Bishops() & rep.own_pieces;
        GetSliderMoves(bishops, ~rep.enemy_pieces, rep, move_list, AttackTables::BishopAttacks);
    }

    void GetQuietMoves(const Board::Representation& representation, Board::CastlingRights rights, MoveList& move_list){
        GetPawnQuietMoves(representation, move_list);
        GetKnightQuietMoves(representation, move_list);
        GetBishopQuietMoves(representation, move_list);
        GetRookQuietMoves(representation, move_list);
        GetQueenQuietMoves(representation, move_list);
        GetKingQuietMoves(representation, rights, move_list);
    }

    void GetCaptures(const Board::Representation& representation, MoveList& move_list){
        GetPawnCaptures(representation, move_list);
        GetKnightCaptures(representation, move_list);
        GetBishopCaptures(representation, move_list);
        GetRookCaptures(representation, move_list);
        GetQueenCaptures(representation, move_list);
        GetKingCaptures(representation, move_list);
    }


}