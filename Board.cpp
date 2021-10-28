#include "Board.h"

namespace ChessEngine {

    void Representation::Mirror() {
        rook_queens.Mirror();
        bishop_queens.Mirror();
        pawns_enPassant.Mirror();
        own_King.Mirror();
        enemy_King.Mirror();

        own_Pieces.Mirror();
        enemy_Pieces.Mirror();

        std::swap(own_Pieces, enemy_Pieces);
        std::swap(own_King, enemy_King);
    }

    Board::Board(const BoardInfo& info, Team startingTeam) :
            representation_(std::get<Representation>(info)),
            castlingRights_(std::get<CastlingRights>(info)),
            moveCounters_(std::get<MoveCounters>(info)),
            isFlipped_(startingTeam == Team::Black){
        // If black starts the board should be flipped.
        if(isFlipped_)
            Mirror();
    }

    void Board::Mirror() {
        representation_.Mirror();
        castlingRights_.Mirror();
        isFlipped_ = !isFlipped_;
    }

}