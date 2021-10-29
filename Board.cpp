#include "Board.h"

namespace ChessEngine {

    void Representation::Mirror() {
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

}