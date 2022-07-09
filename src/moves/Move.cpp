#include "Move.h"

namespace ChessEngine{

    std::string Move::AlgebraicNotation(bool is_flipped) const{
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

    Move::Move(std::string algebraic_notation, bool is_flipped){
        // NOTE: Assumes string is correct.
        // Also be ware that this is from white perspective.
        // Any needed flipping should be done.

        std::string from_string = std::string(&algebraic_notation[0], &algebraic_notation[2]);
        std::string to_string = std::string(&algebraic_notation[2], &algebraic_notation[4]);

        std::tuple<uint8_t, uint8_t> coords_from;
        NotationToCoords(from_string, coords_from);
        auto [from_file, from_rank] = coords_from;
        BoardTile from(from_file, from_rank);

        std::tuple<uint8_t, uint8_t> coords_to;
        NotationToCoords(to_string, coords_to);
        auto [to_file, to_rank] = coords_to;
        BoardTile to(to_file, to_rank);

        if(is_flipped){
            from.Mirror();
            to.Mirror();
        }

        if(algebraic_notation.size() > 4){
            // Add promotion.
            PieceInfo info;
            CharToPieceInfo(algebraic_notation[4], info);
            auto [piece_type, team] = info;
            *this = Move(from, to, piece_type);
        }else{
            *this = Move(from, to, PieceType::None);
        }
    }

}