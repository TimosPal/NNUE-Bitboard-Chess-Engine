#include "NNUE.h"

#include "nnue-probe/nnue.h"

namespace ChessEngine{

    int GetSide(const Team& team){
        return team == Team::White ? 0 : 1;
    }

    int GetSide(bool is_flipped){
        return !is_flipped ? 0 : 1;
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

    int GetPieceEncoding(const ChessEngine::PieceInfo& info){
        auto [type, team] = info;
        return GetPieceEncoding(type, team);
    }

    int GetSquare(const BoardTile& tile){
        // A1=0, B1=1 ... H8=63
        // Already calculated as so.
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

        InitInput(representation, is_flipped, pieces, squares);
        int side = GetSide(is_flipped);

        NNUEdata* a_nnue[3] = {0, 0, 0};
        //for(int i = 0; i < 3 && hply >= i; i++)
        //a_nnue[i] = &nnue[hply - i];

        NNUEdata nnue_t;
        nnue_t.accumulator.computedAccumulation = 0;

        a_nnue[0] = &nnue_t;
        return nnue_evaluate_incremental(side, pieces, squares, &a_nnue[0]);
    }

    void InitModel(char* file_name){
        nnue_init(file_name);
    }

}