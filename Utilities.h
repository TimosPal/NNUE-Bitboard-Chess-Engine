#ifndef ENGINE_UTIL_H
#define ENGINE_UTIL_H

#include <string>
#include <tuple>
#include "Bitboard.h"

#define IF_ERROR(cond, msg) {if(cond) { std::cout << "[ERROR] " << msg << std::endl; return 1;}}

namespace ChessEngine {

    /* Not using eum classes for easy indexing */

    enum Team {
        White, Black
    };

    enum File{
        A, B, C, D, E, F, G, H
    };

    enum Rank{
        R1, R2, R3, R4, R5, R6, R7, R8
    };

    enum PieceType{
        None, King, Queen, Bishop, Knight, Rook, Pawn
    };

    enum class GameResult{
        WhiteWon, BlackWon, Draw, Playing
    };

    using PieceInfo = std::tuple<PieceType, Team>;

    namespace Masks{
        // Files.
        const Bitboard file_A = 0x0101010101010101ULL;
        const Bitboard file_B = file_A << 1;
        const Bitboard file_G = file_A << 6;
        const Bitboard file_H = file_A << 7;

        // Not files.
        const Bitboard not_file_A = ~file_A;
        const Bitboard not_file_B = ~file_B;
        const Bitboard not_file_G = ~file_G;
        const Bitboard not_file_H = ~file_H;
        const Bitboard not_file_AB = ~(file_A | file_B);
        const Bitboard not_file_GH = ~(file_H | file_G);

        // Ranks.
        const Bitboard rank_1 = 0xff;
        const Bitboard rank_2 = rank_1 << (8 * 1);
        const Bitboard rank_3 = rank_1 << (8 * 2);
        const Bitboard rank_6 = rank_1 << (8 * 5);
        const Bitboard rank_7 = rank_1 << (8 * 6);
        const Bitboard rank_8 = rank_1 << (8 * 7);

        const Bitboard outer_tiles = rank_1 | rank_8  | file_A | file_H;
        const Bitboard inner_tiles = ~outer_tiles;

        // Corners.
        const BoardTile a1_tile = BoardTile(0,0);
        const BoardTile a8_tile = BoardTile(0,7);
        const BoardTile h1_tile = BoardTile(7,0);
        const BoardTile h8_tile = BoardTile(7,7);
        const Bitboard corner_tiles = a1_tile | a8_tile | h1_tile | h8_tile;
    }

    bool CharToPieceInfo(char token, PieceInfo &piece_info);
    char PieceInfoToChar(const PieceInfo& piece_info);

    bool NotationToCoords(const std::string &str, std::tuple<uint8_t, uint8_t>& coords);
    bool CoordsToNotation(const std::tuple<uint8_t, uint8_t>& coords, std::string &str);


}

#endif
