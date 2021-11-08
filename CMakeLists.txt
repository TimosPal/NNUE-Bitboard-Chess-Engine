cmake_minimum_required(VERSION 3.16.3)
project(ChessEngine2)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_DEBUG "-O3")

# set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

add_executable(ChessEngine2
        main.cpp
        Bitboard.h
        Bitboard.cpp
        Board.cpp
        Board.h
        Move.h
        FenParser.h
        FenParser.cpp
        Utilities.cpp
        Utilities.h
        AttackTables.cpp
        AttackTables.h
        MagicNumbers.h PseudoMoves.cpp PseudoMoves.h BoardTile.h Timer.cpp Timer.h)

target_compile_definitions(ChessEngine2 PRIVATE PROFILER)