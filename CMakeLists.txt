cmake_minimum_required(VERSION 3.16.3)
project(ChessEngine2)

set(CMAKE_CXX_STANDARD 20)

#set(CMAKE_CXX_FLAGS "-Wall -Wextra")
#set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_DEBUG "-Ofast")

#set (CMAKE_CXX_FLAGS "-mavx2 -msse4.1 -msse3 -msse2 -msse")
set (CMAKE_CXX_FLAGS "-msse4.1 -msse3 -msse2 -msse")

set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

add_executable(Tim_ChessEngine
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
        MagicNumbers.h
        PseudoMoves.cpp
        PseudoMoves.h
        BoardTile.h
        Timer.cpp
        Timer.h
        nnue-probe/misc.cpp
        nnue-probe/misc.h
        nnue-probe/nnue.cpp
        nnue-probe/nnue.h
        NNUE.h
        NNUE.cpp Search.h
        Search.cpp
        ZobristKey.h
        ZobristKey.cpp
        TranspositionTable.h
        TranspositionTable.cpp
        UCI.h
        Move.cpp
        UCI.cpp
        History.h
        MCTS.h MCTS.cpp)

target_compile_definitions(Tim_ChessEngine PRIVATE PROFILER)
target_compile_definitions(Tim_ChessEngine PRIVATE NDEBUG)
#target_compile_definitions(Tim_ChessEngine PRIVATE USE_AVX2 USE_SSE41 USE_SSE3 USE_SSE2 USE_SSE)
target_compile_definitions(Tim_ChessEngine PRIVATE USE_SSE41 USE_SSE3 USE_SSE2 USE_SSE)