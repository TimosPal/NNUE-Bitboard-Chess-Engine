#include <cassert>
#include "Board.h"

namespace ChessEngine {

    void Board::Representation::Mirror() {
        rook_queens.Mirror();
        bishop_queens.Mirror();
        pawns_enPassant.Mirror();
        own_king.Mirror();
        enemy_king.Mirror();

        own_pieces.Mirror();
        enemy_pieces.Mirror();

        std::swap(own_pieces, enemy_pieces);
        std::swap(own_king, enemy_king);
    }

    Board::Board(const BoardInfo &info) :
            representation_(std::get<Representation>(info)),
            castling_rights_(std::get<CastlingRights>(info)),
            move_counters_(std::get<MoveCounters>(info)),
            is_flipped_(std::get<Team>(info) == Team::Black)
            {}

    void Board::Mirror() {
        representation_.Mirror();
        castling_rights_.Mirror();
        is_flipped_ = !is_flipped_;
    }

    void Board::PlayMove(Move move){
        BoardTile from = move.GetFrom();
        BoardTile to = move.GetTo();
        PieceType promotion = move.GetPromotion();

        // Reminder that coords start at {0,0}.
        auto[from_file, from_rank] = from.GetCoords();
        auto[to_file, to_rank] = to.GetCoords();

        // Reset , set global occupancies.
        representation_.own_pieces.Reset(from);
        representation_.own_pieces.Set(to);
        representation_.enemy_pieces.Reset(to);

        // Reset captured pieces.
        representation_.rook_queens.Reset(to);
        representation_.bishop_queens.Reset(to);
        representation_.pawns_enPassant.Reset(to);
        representation_.enemy_pieces.Reset(to);

        // Castling.
        if(from == representation_.own_king){
            // Reset rights since king moved or castled.
            castling_rights_.ResetOwnKingSide();
            castling_rights_.ResetOwnQueenSide();

            auto castling = [this](BoardTile rook_from, BoardTile rook_to){
                representation_.rook_queens.Reset(rook_from);
                representation_.rook_queens.Set(rook_to);
                representation_.own_pieces.Reset(rook_from);
                representation_.own_pieces.Set(rook_to);
            };

            bool is_starting_position = from == Masks::king_default;
            bool is_queen_side = is_starting_position && to == (Masks::queen_rook + 1);
            bool is_king_side = is_starting_position && to == (Masks::king_rook - 1);
            if(is_queen_side)
                castling(Masks::queen_rook, Masks::queen_rook + 2);
            else if(is_king_side)
                castling(Masks::king_rook,  Masks::king_rook - 2);

            // We dont need to set any other bitboards since that's done for every piece later.
            representation_.own_king = to;
        }
        // A Castling right is reset if a rook moves. We can reset the rights regardless of the piece type.
        else if(from == Masks::queen_rook) {
            castling_rights_.ResetOwnQueenSide();
        } else if(from == Masks::king_rook) {
            castling_rights_.ResetOwnKingSide();
        }
        // Pawn.
        else if(representation_.pawns_enPassant.Get(from)) {
            // Double pawn push. En passant is set at the 0-th rank.
            if ((to_rank - from_rank) == 2) {
                representation_.own_pieces.Set(from_file, 0);
            }
            // En passant capture. Fake pawn does not exist. Movement is diagonal.
            else if (!representation_.enemy_pieces.Get(to) && from_file != to_file) {
                // Enemy pawn is one tile below en passant square.
                BoardTile enemy_pawn = BoardTile(to_file, to_rank - 1);
                representation_.pawns_enPassant.Reset(enemy_pawn);
                representation_.enemy_pieces.Reset(enemy_pawn);
            }
            // Promotions.
            else if(to_rank == 7){
                switch (promotion) {
                    case Queen:
                        representation_.bishop_queens.Set(to);
                        representation_.rook_queens.Set(to);
                        break;
                    case Bishop:
                        representation_.bishop_queens.Set(to);
                        break;
                    case Knight:
                        // Do nothing.                    break;
                    case Rook:
                        representation_.rook_queens.Set(to);
                        break;
                    default:
                        // Invalid promotion.
                        assert(false);
                        break;
                }
            }
            // If nothing of the above is true then it follows the general case.
        }

        // If an enemy rook is captured , reset castling rights.
        const uint8_t enemy_rooks_offset = 7 * 8;
        if(to == Masks::queen_rook + enemy_rooks_offset)
            castling_rights_.ResetEnemyQueenSide();
        if(from == Masks::king_rook + enemy_rooks_offset)
            castling_rights_.ResetEnemyKingSide();

        // Reset old en passant. Does not affect own en passant.
        representation_.pawns_enPassant -= Masks::rank_8;

        // Set bitboard if it is of type [from].
        bool is_promo = promotion != PieceType::None;
        bool is_rook_queen = representation_.rook_queens.Get(from);
        bool is_bishop_queen = representation_.bishop_queens.Get(from);
        bool is_pawn_enPassant = representation_.pawns_enPassant.Get(from);
        representation_.rook_queens.SetIf(to, is_rook_queen);
        representation_.bishop_queens.SetIf(to, is_bishop_queen);
        representation_.pawns_enPassant.SetIf(to, is_pawn_enPassant && !is_promo);

        // Reset from.
        representation_.rook_queens.Reset(from);
        representation_.bishop_queens.Reset(from);
        representation_.pawns_enPassant.Reset(from);

        // Change turn by mirroring the board.
        Mirror();
    }

}