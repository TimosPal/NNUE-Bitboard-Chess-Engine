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
        const Bitboard r1 = 0xff;
        const Bitboard r2 = r1 << (8 * 1);
        const Bitboard r3 = r1 << (8 * 2);
        const Bitboard r6 = r1 << (8 * 5);
        const Bitboard r7 = r1 << (8 * 6);
        const Bitboard r8 = r1 << (8 * 7);
    }

    bool CharToPieceInfo(char token, PieceInfo &piece_info);
    char PieceInfoToChar(const PieceInfo& piece_info);

    bool NotationToCoords(const std::string &str, std::tuple<uint8_t, uint8_t>& coords);
    bool CoordsToNotation(const std::tuple<uint8_t, uint8_t>& coords, std::string &str);


}

#endif
