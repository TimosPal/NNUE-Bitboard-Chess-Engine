#include "NNUE.h"

#include <cstring>

#define CACHE_LINE_SIZE  64
#define MAX_HSTACK 1024

namespace ChessEngine{

    NNUEdata* nnue_data_arr;

    int GetSide(const Team& team){
        return team == Team::White ? 0 : 1;
    }

    int GetSide(bool is_flipped){
        return !is_flipped ? 0 : 1;
    }

    int GetPieceEncoding(const PieceType& type, bool is_flipped){
        return GetPieceEncoding(type, is_flipped ? Black : White);
    }

    int GetPieceEncoding(PieceInfo pieceInfo){
        auto[piece, team] = pieceInfo;
        return GetPieceEncoding(piece, team);
    }

    int GetPieceEncoding(const PieceType& type, const Team& team){
        // Piece codes are
        // wking=1, wqueen=2, wrook=3, wbishop= 4, wknight= 5, wpawn= 6,
        // bking=7, bqueen=8, brook=9, bbishop=10, bknight=11, bpawn=12,
        int encoding;
        switch (type) {
            case None:
                assert(false);
            case King:
                encoding = 1;
                break;
            case Queen:
                encoding = 2;
                break;
            case Rook:
                encoding = 3;
                break;
            case Bishop:
                encoding = 4;
                break;
            case Knight:
                encoding = 5;
                break;
            case Pawn:
                encoding = 6;
                break;
        }

        if(team == Team::Black)
            encoding += 6;

        return encoding;
    }

    int GetSquare(const BoardTile& tile){
        // A1=0, B1=1 ... H8=63
        // Already calculated as so.
        return tile.GetIndex();
    }

    int GetSquare(BoardTile tile, bool is_flipped){
        // A1=0, B1=1 ... H8=63
        // Already calculated as so.
        if(is_flipped)
            tile.Mirror();
        return tile.GetIndex();
    }

    void InitInput(Board::Representation representation, bool is_flipped, int* pieces, int* squares){
        // index 0 -> white king.
        // index 1 -> black king.
        // index x -> piece encoding
        // index n + 1 -> 0 for end of array.
        // Ordering does not matter other than the kings.

        // Make it so own is always white.
        // Also mirrors the tiles.
        if(is_flipped)
            representation.Mirror();

        int index = 0;
        auto update_arrays = [&] (Bitboard piece_board, PieceType type) {
            // White first for kings to work.
            for(auto tile : piece_board & representation.own_pieces){
                squares[index] = GetSquare(tile);
                pieces[index++] = GetPieceEncoding(type, White);
            }
            for(auto tile : piece_board & representation.enemy_pieces){
                squares[index] = GetSquare(tile);
                pieces[index++] = GetPieceEncoding(type, Black);
            }
        };

        update_arrays(representation.Kings(), King);
        update_arrays(representation.Pawns(), Pawn);
        update_arrays(representation.Knights(), Knight);
        update_arrays(representation.Queens(), Queen);
        update_arrays(representation.Rooks(), Rook);
        update_arrays(representation.Bishops(), Bishop);

        pieces[index] = 0;
    }

    int Evaluate(const Board& board){
        const int max_pieces = 16 * 2;
        static int pieces[max_pieces + 1];
        static int squares[max_pieces];

        Board::Representation representation = board.GetRepresentation();
        bool is_flipped = board.IsFlipped();

        InitInput(representation, is_flipped, pieces, squares);
        int side = GetSide(is_flipped);
        int eval = nnue_evaluate(side, pieces, squares);

        return eval;
    }

    int EvaluateIncremental(const Board& board){
        const int max_pieces = 16 * 2;
        static int pieces[max_pieces + 1];
        static int squares[max_pieces];

        Board::Representation representation = board.GetRepresentation();
        bool is_flipped = board.IsFlipped();

        // TODO: nnue prob does not need this for most of the calls...
        InitInput(representation, is_flipped, pieces, squares);
        int side = GetSide(is_flipped);

        uint8_t current_ply = board.GetPlyCounter() - 1;
        NNUEdata* nnue_latest_data[3] = {0, 0, 0};
        for(int i = 0; i < 3 && current_ply >= i; i++)
            nnue_latest_data[i] = &nnue_data_arr[current_ply - i];

        return nnue_evaluate_incremental(side, pieces, squares, &nnue_latest_data[0]);
    }

    void InitAccumulator(int ply){
        nnue_data_arr[ply].accumulator.computedAccumulation = 0;
    }

    DirtyPiece* GetDirtyPiece(int ply){
        return &(nnue_data_arr[ply].dirtyPiece);
    }

    void CopyToNextAccumulator(int ply){
        assert(ply - 1 >= 0);
        memcpy(&nnue_data_arr[ply].accumulator, &nnue_data_arr[ply - 1].accumulator, sizeof(Accumulator));
        DirtyPiece* dp = &(nnue_data_arr[ply].dirtyPiece);
        dp->dirtyNum = 0;
    }

    template<typename T, int ALIGNMENT = CACHE_LINE_SIZE, bool large_pages = false>
    void aligned_reserve(T*& mem,const size_t& size) {
        #ifdef __ANDROID__
                mem = (T*) memalign(ALIGNMENT,size * sizeof(T));
        #elif defined(_WIN32)
                mem = (T*)_aligned_malloc(size * sizeof(T),ALIGNMENT);
        #else
                posix_memalign((void**)&mem,ALIGNMENT,size * sizeof(T));
        #if defined(MADV_HUGEPAGE)
            if(large_pages)
                madvise(mem,size * sizeof(T),MADV_HUGEPAGE);
        #endif
        #endif
    }

    void InitModel(char* file_name){
        nnue_init(file_name);
        aligned_reserve<NNUEdata>(nnue_data_arr, MAX_HSTACK);
    }

}