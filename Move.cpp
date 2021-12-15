#include "Move.h"

namespace ChessEngine{

    std::string Move::AlgebraicNotation(bool is_flipped){
        // Returns a string with formal algebraic notation.
        // The board is flipped accordingly.
        auto from = ChessEngine::BoardTile(this->GetFrom());
        auto to = ChessEngine::BoardTile(this->GetTo());
        if(is_flipped){
            from.Mirror();
            to.Mirror();
        }

        std::string from_coords_str;
        ChessEngine::CoordsToNotation(from.GetCoords(), from_coords_str);
        std::string to_coords_str;
        ChessEngine::CoordsToNotation(to.GetCoords(), to_coords_str);
        std::string promotion_str;
        ChessEngine::PieceType promotion = this->GetPromotion();
        if(promotion != ChessEngine::None){
            promotion_str = std::string(1, ChessEngine::PieceInfoToChar({promotion, ChessEngine::White}));
        }

        std::ostringstream oss;
        oss << from_coords_str << to_coords_str << promotion_str;
        return oss.str();
    }

}