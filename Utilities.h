#ifndef ENGINE_UTIL_H
#define ENGINE_UTIL_H

#include <assert.h>
#include <iostream>
#include <string>
#include <tuple>
#include <chrono>
#include <unordered_map>

#include "Bitboard.h"
#include "BoardTile.h"

#define IF_ERROR(cond, msg) {if(cond) { std::cout << "[ERROR] " << msg << std::endl; return 1;}}
#define PROFILE_SCOPE(name) ChessEngine::Timer timer##__LINE__(name)
#define PROFILE_FUNCTION() PROFILE_SCOPE(__FUNCTION__)

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
        const Bitboard file_A = Bitboard(0x0101010101010101ULL);
        const Bitboard file_B = file_A.ShiftTowards({1,0});
        const Bitboard file_G = file_A.ShiftTowards({6,0});
        const Bitboard file_H = file_A.ShiftTowards({7,0});

        // Not files.
        const Bitboard not_file_A = ~file_A;
        const Bitboard not_file_B = ~file_B;
        const Bitboard not_file_G = ~file_G;
        const Bitboard not_file_H = ~file_H;
        const Bitboard not_file_AB = ~(file_A | file_B);
        const Bitboard not_file_GH = ~(file_H | file_G);

        // Ranks.
        const Bitboard rank_1 = Bitboard(0xff);
        const Bitboard rank_2 = rank_1.ShiftTowards({0,1});
        const Bitboard rank_3 = rank_1.ShiftTowards({0,2});
        const Bitboard rank_4 = rank_1.ShiftTowards({0,3});
        const Bitboard rank_6 = rank_1.ShiftTowards({0,5});
        const Bitboard rank_7 = rank_1.ShiftTowards({0,6});
        const Bitboard rank_8 = rank_1.ShiftTowards({0,7});
        const Bitboard rank_1_8 = rank_1 | rank_8;

        const Bitboard outer_tiles = rank_1 | rank_8  | file_A | file_H;
        const Bitboard inner_tiles = ~outer_tiles;
        const Bitboard empty = Bitboard(0);

        // Corners.
        const BoardTile a1_tile = BoardTile(0,0);
        const BoardTile a8_tile = BoardTile(0,7);
        const BoardTile h1_tile = BoardTile(7,0);
        const BoardTile h8_tile = BoardTile(7,7);
        const Bitboard corner_tiles = a1_tile | a8_tile | h1_tile | h8_tile;

        // Castling.
        const BoardTile king_default = BoardTile(4, 0);
        const BoardTile queen_rook = BoardTile(0,0);
        const BoardTile king_rook = BoardTile(7,0);
        const Bitboard queen_castling_tiles = BoardTile(1,0) | BoardTile(2,0) | BoardTile(3,0);
        const Bitboard king_castling_tiles = BoardTile(6,0) | BoardTile(5,0);
    }

    bool CharToPieceInfo(char token, PieceInfo &piece_info);
    char PieceInfoToChar(const PieceInfo& piece_info);
    std::string PieceInfoToAscii(const PieceInfo& piece_info);

    bool NotationToCoords(const std::string &str, std::tuple<uint8_t, uint8_t>& coords);
    bool CoordsToNotation(const std::tuple<uint8_t, uint8_t>& coords, std::string &str);

    class Timer{
    public:
        struct TimerInfo{
            long long int time_sum = 0;
            int number_of_calls = 0;
        };

        Timer(std::string function_name);
        ~Timer(){ Stop(); }
        void Stop();
        static void Print();
    private:
        std::chrono::time_point<std::chrono::steady_clock> starting_time_point_;
        std::string function_name_;

        static std::unordered_map<std::string, TimerInfo> info;
    };

}

#endif
