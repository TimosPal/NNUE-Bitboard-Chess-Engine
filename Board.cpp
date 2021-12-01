#include "Board.h"

#include <cassert>
#include <algorithm>

#include "PseudoMoves.h"
#include "AttackTables.h"

#include "NNUE.h"

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

    void Board::UnPlayMove(Move move, PieceType captured_piece){ // TODO: WIP.
        BoardTile from = move.GetFrom();
        BoardTile to = move.GetTo();

        representation_.enemy_pieces.Reset(to);
        representation_.enemy_pieces.SetIf(to, captured_piece != PieceType::None);
        representation_.own_pieces.Reset(to);
        representation_.own_pieces.Set(from);

        bool is_promo = move.GetPromotion() != None;
        bool is_rook_queen = representation_.rook_queens.Get(to);
        bool is_bishop_queen = representation_.bishop_queens.Get(to);
        bool is_pawn = representation_.pawns_enPassant.Get(to);
        representation_.rook_queens.SetIf(from, !is_promo && is_rook_queen);
        representation_.bishop_queens.SetIf(from, !is_promo && is_bishop_queen);
        representation_.pawns_enPassant.SetIf(from, is_promo || is_pawn);

        representation_.rook_queens.SetIf(to, captured_piece == Rook || captured_piece == Queen);
        representation_.bishop_queens.SetIf(to, captured_piece == Bishop || captured_piece == Queen);
        representation_.pawns_enPassant.SetIf(to, captured_piece == Pawn);

        if(to == representation_.own_king)
            representation_.own_king = from;

        // Captures.

        // Promotions.

        // En passant.

        // Castling.

        // Castling rights (ours , or enemy's)
    }

    void Board::PlayMove(Move move){
        BoardTile from = move.GetFrom();
        BoardTile to = move.GetTo();
        PieceType promotion = move.GetPromotion();

        // NNUE move data.
        InitAccumulator(move_counters_.ply_counter);
        DirtyPiece* dirty_piece = GetDirtyPiece(move_counters_.ply_counter);
        dirty_piece->dirtyNum = 1;

        Team own_color, enemy_color;
        if(is_flipped_){
            own_color = Black;
            enemy_color = White;
        }else{
            own_color = White;
            enemy_color = Black;
        }

        auto[own_piece_type, own_team] = GetPieceInfoAt(from);
        dirty_piece->pc[0] = GetPieceEncoding(own_piece_type, own_color);
        dirty_piece->from[0] = GetSquare(from, is_flipped_);
        dirty_piece->to[0] = GetSquare(to, is_flipped_);

        if(representation_.enemy_pieces.Get(to)){
            dirty_piece->dirtyNum = 2;
            auto[enemy_piece_type, enemy_team] = GetPieceInfoAt(to);
            dirty_piece->pc[1] = GetPieceEncoding(enemy_piece_type, enemy_color);
            dirty_piece->from[1] = GetSquare(to, is_flipped_);
            dirty_piece->to[1] = REMOVED_SQUARE;
        }

        // Reminder that coords start at {0,0}.
        auto[from_file, from_rank] = from.GetCoords();
        auto[to_file, to_rank] = to.GetCoords();

        // Reset captured pieces.
        representation_.rook_queens.Reset(to);
        representation_.bishop_queens.Reset(to);
        representation_.pawns_enPassant.Reset(to);

        // If there has been a capture.
        bool reset_50_move_rule = representation_.enemy_pieces.Get(to);

        // King.
        if(from == representation_.own_king){
            // Reset rights since king moved or castled.
            castling_rights_.ResetOwnKingSide();
            castling_rights_.ResetOwnQueenSide();

            auto castling = [&](BoardTile rook_from, BoardTile rook_to){
                dirty_piece->dirtyNum = 2;
                dirty_piece->from[1] = GetSquare(rook_from, is_flipped_);
                dirty_piece->to[1] = GetSquare(rook_to, is_flipped_);
                dirty_piece->pc[1] = GetPieceEncoding(PieceType::Rook, own_color);

                representation_.rook_queens.Reset(rook_from);
                representation_.rook_queens.Set(rook_to);
                representation_.own_pieces.Reset(rook_from);
                representation_.own_pieces.Set(rook_to);
            };

            bool is_castling = abs(from_file - to_file) == 2;
            bool is_queen_side = is_castling && to == (Masks::queen_rook + 2);
            bool is_king_side = is_castling && to == (Masks::king_rook - 1);
            if(is_queen_side)
                castling(Masks::queen_rook, Masks::queen_rook + 3);
            else if(is_king_side)
                castling(Masks::king_rook,  Masks::king_rook - 2);

            representation_.own_king = to;
        }
        // A Castling right is reset if a rook moves. We can reset the rights regardless of the piece type.
        else if(from == Masks::queen_rook) {
            castling_rights_.ResetOwnQueenSide();
        } else if(from == Masks::king_rook) {
            castling_rights_.ResetOwnKingSide();
        }
        // Pawn.
        else if(representation_.Pawns().Get(from)) {
            // Pawn movements reset the rule.
            reset_50_move_rule = true;

            // Double pawn push. En passant is set at the 0-th rank.
            if ((to_rank - from_rank) == 2) {
                representation_.pawns_enPassant.Set(from_file, 0);
            }
            // En passant capture. Fake pawn does not exist. Movement is diagonal.
            else if (!representation_.enemy_pieces.Get(to) && from_file != to_file) {
                // Enemy pawn is one tile below en passant square.
                BoardTile enemy_pawn = BoardTile(to_file, to_rank - 1);
                representation_.pawns_enPassant.Reset(enemy_pawn);
                representation_.enemy_pieces.Reset(enemy_pawn);

                dirty_piece->dirtyNum = 2;
                dirty_piece->pc[1] = GetPieceEncoding(PieceType::Pawn, enemy_color);
                dirty_piece->from[1] = GetSquare(enemy_pawn, is_flipped_);
                dirty_piece->to[1] = REMOVED_SQUARE;
            }
            // Promotions.
            else if(to_rank == Rank::R8){
                dirty_piece->to[0] = REMOVED_SQUARE;
                dirty_piece->from[dirty_piece->dirtyNum] = REMOVED_SQUARE;
                dirty_piece->to[dirty_piece->dirtyNum] = GetSquare(to, is_flipped_);

                switch (promotion) {
                    case Queen:
                        representation_.bishop_queens.Set(to);
                        representation_.rook_queens.Set(to);
                        dirty_piece->pc[dirty_piece->dirtyNum] = GetPieceEncoding(PieceType::Queen, own_color);
                        break;
                    case Bishop:
                        representation_.bishop_queens.Set(to);
                        dirty_piece->pc[dirty_piece->dirtyNum] = GetPieceEncoding(PieceType::Bishop, own_color);
                        break;
                    case Rook:
                        representation_.rook_queens.Set(to);
                        dirty_piece->pc[dirty_piece->dirtyNum] = GetPieceEncoding(PieceType::Rook, own_color);
                        break;
                    case Knight:
                        dirty_piece->pc[dirty_piece->dirtyNum] = GetPieceEncoding(PieceType::Knight, own_color);
                        break;
                    default:
                        // Invalid promotion.
                        assert(false);
                        break;

                }
                dirty_piece->dirtyNum++;
            }
        }

        // If an enemy rook is captured , reset castling rights.
        const uint8_t enemy_rooks_offset = 7 * 8;
        if(to == Masks::queen_rook + enemy_rooks_offset)
            castling_rights_.ResetEnemyQueenSide();
        else if(to == Masks::king_rook + enemy_rooks_offset)
            castling_rights_.ResetEnemyKingSide();

        // Reset old en passant. Does not affect own en passant.
        representation_.pawns_enPassant -= Masks::rank_8;

        // Reset , set global occupancies.
        representation_.own_pieces.Reset(from);
        representation_.own_pieces.Set(to);
        representation_.enemy_pieces.Reset(to);

        // Set bitboard if it is of type [from].
        bool is_promo = promotion != PieceType::None;
        bool is_rook_queen = representation_.rook_queens.Get(from);
        bool is_bishop_queen = representation_.bishop_queens.Get(from);
        bool is_pawn = representation_.pawns_enPassant.Get(from);
        representation_.rook_queens.SetIf(to, is_rook_queen);
        representation_.bishop_queens.SetIf(to, is_bishop_queen);
        representation_.pawns_enPassant.SetIf(to, is_pawn && !is_promo);

        // Reset from.
        representation_.rook_queens.Reset(from);
        representation_.bishop_queens.Reset(from);
        representation_.pawns_enPassant.Reset(from);

        // Set counters,
        if(is_flipped_)
            // Increments only when black moves.
            move_counters_.full_moves++;

        // Increments only if there were no captures or pawn moves.
        if(reset_50_move_rule)
            move_counters_.half_moves = 0;
        else
            move_counters_.half_moves++;
        move_counters_.ply_counter++;
    }

    bool Board::IsUnderAttack(BoardTile tile) const{
        // Consider the tile a piece with all the possible moves.
        Bitboard own = representation_.own_pieces;
        Bitboard enemy = representation_.enemy_pieces;
        Bitboard all = own | enemy;
        uint8_t tile_index = tile.GetIndex();

        Bitboard king_attacks = AttackTables::KingAttacks(tile_index) & Bitboard(representation_.enemy_king);
        if(!king_attacks.IsEmpty())
            return true;
        Bitboard rook_attacks = AttackTables::RookAttacks(tile_index, all) & enemy & representation_.rook_queens;
        if(!rook_attacks.IsEmpty())
            return true;
        Bitboard bishop_attacks = AttackTables::BishopAttacks(tile_index, all) & enemy & representation_.bishop_queens;
        if(!bishop_attacks.IsEmpty())
            return true;
        Bitboard knight_attacks = AttackTables::KnightAttacks(tile_index) & enemy & representation_.Knights();
        if(!knight_attacks.IsEmpty())
            return true;
        Bitboard pawn_attacks = AttackTables::PawnsAttacks(tile_index) & enemy & representation_.pawns_enPassant;
        if(!pawn_attacks.IsEmpty())
            return true;

        return false;
    }

    bool Board::IsInCheck() const{
        return IsUnderAttack(representation_.own_king);
    }

    Bitboard Board::GetPins() const{
        Bitboard pins(0);

        Bitboard own = representation_.own_pieces;
        Bitboard enemy = representation_.enemy_pieces;
        Bitboard bishops = representation_.bishop_queens;
        Bitboard rooks = representation_.rook_queens;
        Bitboard all = own | enemy;

        uint8_t king_index = representation_.own_king.GetIndex();
        Bitboard rook_pins = AttackTables::RookAttacks(king_index, all) & own;
        Bitboard rook_rays = AttackTables::RookAttacks(king_index);
        Bitboard bishop_pins = AttackTables::BishopAttacks(king_index, all) & own;
        Bitboard bishop_rays = AttackTables::BishopAttacks(king_index);

        for(auto piece : rook_pins){
            uint8_t pinned_piece_index = piece.GetIndex();
            auto mask = rook_rays & enemy & rooks;
            Bitboard rook_attacks = AttackTables::RookAttacks(pinned_piece_index, all) & mask;
            pins.SetIf(piece, !rook_attacks.IsEmpty());
        }

        for(auto piece : bishop_pins){
            uint8_t pinned_piece_index = piece.GetIndex();
            auto mask = bishop_rays & enemy & bishops;
            Bitboard rook_attacks = AttackTables::BishopAttacks(pinned_piece_index, all) & mask;
            pins.SetIf(piece, !rook_attacks.IsEmpty());
        }

        return pins;
    }

    bool Board::IsLegalMove(const Move& move, const Bitboard& pins, bool is_in_check) const {
        // NOTE: can optimize en passant and check positions but it didnt seem to give
        // a great performance boost in perft so the simplified version is kept.
        auto try_move = [=]() {
            Board temp = *this;
            temp.PlayMove(move);
            return !temp.IsInCheck();
        };

        BoardTile from = move.GetFrom();
        BoardTile to = move.GetTo();

        uint8_t from_file = from.GetFile();
        uint8_t to_file = to.GetFile();

        bool is_king = from == representation_.own_king;
        bool is_castling = is_king && (abs(from_file - to_file) == 2);
        bool is_enPassant = to_file != from_file &&
                !representation_.enemy_pieces.Get(to) &&
                representation_.pawns_enPassant.Get(from);

        if(is_enPassant)
            return try_move();

        if(is_in_check){
            if(is_castling)
                return false;
            return try_move();
        }

        if(is_king){
            // Check in between tile.
            if(is_castling && IsUnderAttack(from + (to_file - from_file) / 2))
                return false;
            return try_move();
        }

        if(pins.Get(from)) {
            uint8_t from_rank = from.GetRank();
            uint8_t to_rank = to.GetRank();
            auto[king_file, king_rank] = representation_.own_king.GetCoords();

            const int dx_from = from_file - king_file;
            const int dy_from = from_rank - king_rank;
            const int dx_to = to_file - king_file;
            const int dy_to = to_rank - king_rank;

            // The move is legal only if the vector {from-king} is codirectional with
            // the vector {to-king}. This is checked by comparing the slopes.
            // dy_from / dx_from = dy_to / dx_to.
            if (dx_from == 0 || dx_to == 0) {
                return (dx_from == dx_to);
            } else {
                return (dx_from * dy_to == dx_to * dy_from);
            }
        }

        return true; // Not pinned , no check. Can freely move.
    }

    MoveList Board::GetLegalQuietMoves() const {
        MoveList moves;
        moves.reserve(60);
        PseudoMoves::GetQuietMoves(representation_, castling_rights_, moves);

        Bitboard pins = GetPins();
        bool is_in_check = IsInCheck();
        auto is_illegal = [=](const Move &move) { return !IsLegalMove(move, pins, is_in_check); };
        moves.erase(std::remove_if(moves.begin(), moves.end(), is_illegal), moves.end());

        return moves;
    }

    MoveList Board::GetLegalCaptures() const { // TODO : check for possible refactors.
        // Pre allocate vector size (Requires a Move default constructor).
        MoveList moves;
        moves.reserve(20);
        PseudoMoves::GetCaptures(representation_, moves);

        Bitboard pins = GetPins();
        bool is_in_check = IsInCheck();
        auto is_illegal = [=](const Move &move) { return !IsLegalMove(move, pins, is_in_check); };
        moves.erase(std::remove_if(moves.begin(), moves.end(), is_illegal), moves.end());

        return moves;
    }

    PieceInfo Board::GetPieceInfoAt(BoardTile tile) const{
        auto[file, rank] = tile.GetCoords();
        return GetPieceInfoAt(file, rank);
    }

    PieceInfo Board::GetPieceInfoAt(uint8_t file, uint8_t rank) const {
        PieceType type = GetPieceTypeAt(file, rank);

        Team team;
        if(representation_.own_pieces.Get(file, rank)){
            team = White;
        }else{
            team = Black;
        }

        return {type, team};
    }

    PieceType Board::GetPieceTypeAt(uint8_t file, uint8_t rank) const {
        Bitboard pawns = representation_.Pawns();
        Bitboard knights = representation_.Knights();
        Bitboard rooks = representation_.Rooks();
        Bitboard bishops = representation_.Bishops();
        Bitboard queens = representation_.Queens();
        Bitboard kings = representation_.Kings();

        PieceType type;
        if(pawns.Get(file, rank)){
            type = PieceType::Pawn;
        } else if(knights.Get(file, rank)){
            type = PieceType::Knight;
        } else if(rooks.Get(file, rank)){
            type = PieceType::Rook;
        } else if(bishops.Get(file, rank)){
            type = PieceType::Bishop;
        } else if(queens.Get(file, rank)){
            type = PieceType::Queen;
        } else if(kings.Get(file, rank)) {
            type = PieceType::King;
        } else{
            type = PieceType::None;
        }

        return type;
    }

    void Board::Draw() const{
        for (int rank = 7; rank >= 0; rank--) {
            std::cout << rank + 1 << "  ";
            for (int file = 0; file < 8; file++) {
                 std::cout << PieceInfoToChar(GetPieceInfoAt(file, rank)) << "  ";
            }
            std::cout << std::endl;
        }
        std::cout << "   a  b  c  d  e  f  g  h" << std::endl;
    }

    bool Board::InsufficientMaterial() const{
        // Pawns can promote.
        if(!representation_.pawns_enPassant.IsEmpty())
            return false;
        // Rook or queens can lead to a checkmate.
        if(!representation_.rook_queens.IsEmpty())
            return false;

        // Can not check with 1 minor piece.
        if((representation_.own_pieces | representation_.enemy_pieces).Count() < 4)
            return true;

        // More than 3 pieces exist that are not rook or queens.
        if(!representation_.Knights().IsEmpty())
            return false;

        // Only bishops remain. If they are same color it's a draw.
        Bitboard light_bishops = representation_.bishop_queens & Masks::light_squares;
        Bitboard dark_bishops = representation_.bishop_queens & Masks::dark_squares;
        return light_bishops.IsEmpty() || dark_bishops.IsEmpty();
    }

    GameResult Board::Result(const MoveList& moves) const {
        if(moves.empty()){
            if(IsInCheck()){
                // checkmate.
                return (is_flipped_) ? GameResult::WhiteWon : GameResult::BlackWon;
            }else{
                // stalemate.
                return GameResult::Draw;
            }
        }

        // not enough pieces.
        if(InsufficientMaterial())
            return GameResult::Draw;

        // 50 moves rule.
        if(move_counters_.half_moves >= 100)
            return GameResult::Draw;

        // 3 move repetition.
        // TODO

        return GameResult::Playing;
    }

    std::string Board::Fen() const { // TODO : castling rights moves etc.
        std::string fen;
        for (int rank = 7; rank >= 0; rank--) {
            char empty_tiles = 0;
            for (int file = 0; file < 8; file++) {
                auto[type, team] = GetPieceInfoAt(file, rank);
                if(type != None) {
                    if(empty_tiles != 0){
                        assert(empty_tiles <= 8);
                        fen += std::string(1, empty_tiles + '0');
                    }
                    char token = PieceInfoToChar({type, team});
                    empty_tiles = 0;
                    fen += token;
                }else{
                    empty_tiles++;
                }
            }
            if(empty_tiles != 0){
                assert(empty_tiles <= 8);
                fen += std::string(1, empty_tiles + '0');
            }
            if(rank != 0)
                fen += "/";
        }

        return fen;
    }

}