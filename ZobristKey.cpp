#include "ZobristKey.h"

#include "Board.h"

namespace ChessEngine {
    static uint64_t piece_square_key[16][64];
    static uint64_t enPassant_key[64];
    static uint64_t castling_key[16];
    static uint64_t black_side_key;

    uint64_t GetPieceSquareKey(PieceType type, uint8_t tile_index){
        assert(type != None);
        // We subtract one to exclude none types.
        return piece_square_key[type-1][tile_index];
    }

    uint64_t GetEnPassantKey(uint8_t tile_file){
        return enPassant_key[tile_file];
    }

    uint64_t GetCastlingKey(uint8_t castling){
        return castling_key[castling];
    }

    uint64_t GetSideKey(){
        return black_side_key;
    }


    void InitRandomKeys(){
        auto rand64 = [] () { return ((long long)rand() << 32) | rand(); };
        assert(rand64() >> 32 != 0);

        srand(0);
        for (int type = 0; type < 12; type++) {
            for (int square = 0; square < 64; square++) {
                piece_square_key[type-1][square] = rand64();
            }
        }

        for (int enPassant = 0; enPassant < 64; enPassant++) {
            enPassant_key[enPassant] = rand64();
        }

        for (int castling = 0; castling < 16; castling++) {
            castling_key[castling] = rand64();
        }

        black_side_key = rand64();
    }

    uint64_t ZobristKey(const Board& board, bool is_flipped) {
        const Board::Representation& rep = board.GetRepresentation();
        uint64_t key;

        auto update_key = [&] (Bitboard piece_board, PieceType type){
            for (auto piece : piece_board) {
                bool is_own = rep.own_pieces.Get(piece);
                key ^= piece_square_key[piece.GetIndex() + 6 * is_own][type];
            }
        };

        update_key(rep.Pawns(), PieceType::Pawn);
        update_key(rep.Knights(), PieceType::Knight);
        update_key(rep.Bishops(), PieceType::Bishop);
        update_key(rep.Rooks(), PieceType::Rook);
        update_key(rep.Queens(), PieceType::Queen);
        key ^= piece_square_key[rep.own_king.GetIndex()][PieceType::King];
        key ^= piece_square_key[rep.enemy_king.GetIndex() + 6][PieceType::King];

        // EnPassant can either be in the first 8 first or 8 last bits.
        // For this reason we have 64 positions so we dont bother with the conversion.
        uint8_t enPassant_index = (*Bitboard::Iterator(rep.EnPassant().AsInt())).GetIndex();
        key ^= enPassant_key[enPassant_index];

        key ^= castling_key[board.GetCastlingRights().AsInt()];

        if(is_flipped)
            key ^= black_side_key;

        return key;
    }

}