#include "NNUE.h"

#include "nnue-probe/nnue.h"

namespace ChessEngine{

    int GetSide(const Team& team){
        return team == Team::White ? 0 : 1;
    }

    int GetSide(bool is_flipped){
        return is_flipped ? 0 : 1;
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

    void InitInput(const Board::Representation& representation , int* pieces, int* squares){
        // index 0 -> white king.
        // index 1 -> black king.
        // index x -> piece encoding
        // index n + 1 -> 0 for end of array.
        // Ordering does not matter other than the kings.

        // We always evaluate for white.
        squares[0] = GetSquare(representation.own_king);
        pieces[0] = GetPieceEncoding(King, White);
        squares[1] = GetSquare(representation.enemy_king);
        pieces[1] = GetPieceEncoding(King, Black);

        int index = 2;
        auto update_arrays = [&] (Bitboard piece_board, PieceType type) {
            for(auto tile : piece_board & representation.own_pieces){
                squares[index++] = GetSquare(tile);
                pieces[index++] = GetPieceEncoding(type, White);
            }
            for(auto tile : piece_board & representation.enemy_pieces){
                squares[index++] = GetSquare(tile);
                pieces[index++] = GetPieceEncoding(type, Black);
            }
        };

        update_arrays(representation.Pawns(), Pawn);
        update_arrays(representation.Knights(), Knight);
        update_arrays(representation.Queens(), Queen);
        update_arrays(representation.Rooks(), Rook);
        update_arrays(representation.Bishops(), Bishop);

        pieces[index] = 0;
    }

    int Evaluate(const Board::Representation& representation){
        const int max_pieces = 16 * 2;
        int pieces[max_pieces + 1];
        int squares[max_pieces];

        InitInput(representation, pieces, squares);
        int eval = nnue_evaluate(0, pieces, squares);

        return eval;
    }

}