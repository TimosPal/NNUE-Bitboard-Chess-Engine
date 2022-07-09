#include "ZobristKey.h"

#include <random>

namespace ChessEngine::Zobrist {
    static uint64_t piece_square_key[16][64];
    static uint64_t enPassant_key[8];
    static uint64_t castling_key[16];
    static uint64_t black_side_key;

    uint64_t GetPieceSquareKey(PieceType type, bool is_white, uint8_t tile_index){
        assert(type != None);
        // We subtract one to exclude none type.
        uint8_t piece_index = (type - 1) + 6 * is_white;
        return piece_square_key[piece_index][tile_index];
    }

    uint64_t GetEnPassantKey(uint8_t tile_file){
        return enPassant_key[tile_file];
    }

    uint64_t GetCastlingKey(Board::CastlingRights rights, bool is_flipped){
        if(is_flipped)
            rights.Mirror();
        return castling_key[rights.AsInt()];
    }

    uint64_t GetSideKey(){
        return black_side_key;
    }

    void InitZobristKeysArrays(){
        std::random_device rd;
        std::mt19937_64 e2(rd());
        std::uniform_int_distribution<long long int> dist(0, UINT64_MAX);
        auto rand64 = [&] () { return dist(e2); };

        for (int type = 0; type < 12; type++) {
            for (int square = 0; square < 64; square++) {
                piece_square_key[type][square] = rand64();
            }
        }

        for (int enPassant = 0; enPassant < 8; enPassant++) {
            enPassant_key[enPassant] = rand64();
        }

        for (int castling = 0; castling < 16; castling++) {
            castling_key[castling] = rand64();
        }

        black_side_key = rand64();
    }

    uint64_t GetZobristKey(Board board, bool is_flipped) {
        if(board.IsFlipped())
            board.Mirror();

        const Board::Representation& rep = board.GetRepresentation();
        const Board::CastlingRights& rights = board.GetCastlingRights();

        uint64_t key = 0;
        auto update_key = [&] (Bitboard piece_board, PieceType type){
            for (auto piece : piece_board) {
                bool is_white = rep.own_pieces.Get(piece);
                key ^= GetPieceSquareKey(type, is_white, piece.GetIndex());
            }
        };

        update_key(rep.Pawns(), PieceType::Pawn);
        update_key(rep.Knights(), PieceType::Knight);
        update_key(rep.Bishops(), PieceType::Bishop);
        update_key(rep.Rooks(), PieceType::Rook);
        update_key(rep.Queens(), PieceType::Queen);
        update_key(rep.Kings(), PieceType::King);

        // EnPassant can either be in the first 8 first or 8 last bits.
        // For this reason we have 64 positions so we dont bother with the conversion.
        if(!rep.EnPassant().IsEmpty()) {
            BoardTile enPassant_tile = rep.EnPassant().BitScanForward();
            key ^= GetEnPassantKey(enPassant_tile.GetFile());
        }

        key ^= GetCastlingKey(rights, false);

        if(is_flipped)
            key ^= GetSideKey();

        return key;
    }

}