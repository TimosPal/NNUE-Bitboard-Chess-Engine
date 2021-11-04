#include "Board.h"

#include <cassert>

#include "PseudoMoves.h"
#include "AttackTables.h"

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

        // Reset captured pieces.
        representation_.rook_queens.Reset(to);
        representation_.bishop_queens.Reset(to);
        representation_.pawns_enPassant.Reset(to);

        // King.
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

            bool is_castling = abs(from_file - to_file) >= 2;
            bool is_queen_side = is_castling && to == (Masks::queen_rook + 1);
            bool is_king_side = is_castling && to == (Masks::king_rook - 1);
            if(is_queen_side)
                castling(Masks::queen_rook, Masks::queen_rook + 2);
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
        else if(representation_.pawns_enPassant.Get(from)) {
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
                    case Rook:
                        representation_.rook_queens.Set(to);
                        break;
                    case Knight:
                        // Do nothing.
                        break;
                    default:
                        // Invalid promotion.
                        assert(false);
                        break;
                }
            }
        }

        // If an enemy rook is captured , reset castling rights.
        const uint8_t enemy_rooks_offset = 7 * 8;
        if(to == Masks::queen_rook + enemy_rooks_offset)
            castling_rights_.ResetEnemyQueenSide();
        if(from == Masks::king_rook + enemy_rooks_offset)
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
    }

    bool Board::IsUnderAttack(BoardTile tile){
        // Consider the tile a piece with all the possible moves.
        Bitboard own = representation_.own_pieces;
        Bitboard enemy = representation_.enemy_pieces;
        Bitboard all = own | enemy;
        uint8_t tile_index = tile.GetIndex();

        auto [enemy_king_file, enemy_king_rank] = representation_.enemy_king.GetCoords();
        auto [tile_file, tile_rank] = representation_.own_king.GetCoords();

        // King attacks if he is 1 tile away.
        if(abs(tile_file - enemy_king_file) <= 1 && abs(tile_rank - enemy_king_rank) <= 1)
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

    bool Board::IsInCheck(){
        return IsUnderAttack(representation_.own_king);
    }

    bool Board::IsLegalMove(const Move& move){
        // TODO: some cases can be optimized to not use a copy.
        BoardTile from = move.GetFrom();
        BoardTile to = move.GetTo();

        uint8_t from_file = from.GetFile();
        uint8_t to_file = to.GetFile();

        bool is_king = from == representation_.own_king;
        bool is_castling = is_king && (abs(from_file - to_file) >= 2);
        if(is_castling){
            // Check if already in check.
            if(IsInCheck())
                return false;
            // Check in between tile.
            if(IsUnderAttack(from + (to_file - from_file) / 2))
                return false;
            // End position check will be checked in the general case.
        }

        Board temp = *this;
        temp.PlayMove(move);
        return !temp.IsInCheck();
    }

    MoveList Board::GetLegalMoves(){
        // TODO: use 1 list only. Pre allocate vector size (Requires a Move default constructor).
        MoveList pseudo_moves;
        PseudoMoves::GetPseudoMoves(representation_, castling_rights_, pseudo_moves);

        MoveList legal_moves;
        for(auto move : pseudo_moves)
            if(IsLegalMove(move))
                legal_moves.push_back(move);

        return legal_moves;
    }

    Board::PerftInfo Board::Perft(int depth, Board board){
        int nodes = 0;
        int captures = 0;
        int checks = 0;

        if (depth == 0)
            return {1ULL, 0, 0};

        MoveList moves = board.GetLegalMoves();
        for (int i = 0; i < moves.size(); i++) {
            Board temp = board;

            uint8_t piece_count = temp.representation_.enemy_pieces.Count();
            temp.PlayMove(moves[i]);
            captures += piece_count - temp.representation_.enemy_pieces.Count();
            temp.Mirror();
            if(temp.IsInCheck())
                checks++;

            auto[nodes_next, captures_next, checks_next] = Perft(depth - 1, temp);
            nodes += nodes_next;
            captures += captures_next;
            checks += checks_next;
        }
        return {nodes, captures, checks};
    }

    PieceInfo Board::GetPieceInfoAt(BoardTile tile){
        auto[file, rank] = tile.GetCoords();
        return GetPieceInfoAt(file, rank);
    }

    PieceInfo Board::GetPieceInfoAt(uint8_t file, uint8_t rank) {
        auto representation_temp = representation_;
        if(is_flipped_){
            representation_temp.Mirror();
        }

        Bitboard pawns = representation_temp.Pawns();
        Bitboard knights = representation_temp.Knights();
        Bitboard rooks = representation_temp.Rooks();
        Bitboard bishops = representation_temp.Bishops();
        Bitboard queens = representation_temp.Queens();
        Bitboard kings = representation_temp.Kings();
        Bitboard own = representation_temp.own_pieces;

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

        Team team;
        if(own.Get(file, rank)){
            team = White;
        }else{
            team = Black;
        }

        return {type, team};
    }

    void Board::Draw(){
        for (int rank = 7; rank >= 0; rank--) {
            std::cout << rank + 1 << "  ";
            for (int file = 0; file < 8; file++) {
                 std::cout << PieceInfoToChar(GetPieceInfoAt(file, rank)) << "  ";
            }
            std::cout << std::endl;
        }
        std::cout << "   a  b  c  d  e  f  g  h" << std::endl;
    }

}