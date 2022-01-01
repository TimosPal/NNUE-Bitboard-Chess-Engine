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
        src/main.cpp
        src/Bitboard.h
        src/Bitboard.cpp
        src/Board.cpp
        src/Board.h
        src/Move.h
        src/FenParser.h
        src/FenParser.cpp
        src/Utilities.cpp
        src/Utilities.h
        src/AttackTables.cpp
        src/AttackTables.h
        src/MagicNumbers.h
        src/PseudoMoves.cpp
        src/PseudoMoves.h
        src/BoardTile.h
        src/Timer.cpp
        src/Timer.h
        nnue-probe/misc.cpp
        nnue-probe/misc.h
        nnue-probe/nnue.cpp
        nnue-probe/nnue.h
        src/NNUE.h
        src/NNUE.cpp src/Search.h
        src/Search.cpp
        src/ZobristKey.h
        src/ZobristKey.cpp
        src/TranspositionTable.h
        src/TranspositionTable.cpp
        src/UCI.h
        src/Move.cpp
        src/UCI.cpp
        src/History.h
        src/MCTS.h
        src/MCTS.cpp)

target_compile_definitions(Tim_ChessEngine PRIVATE PROFILER)
target_compile_definitions(Tim_ChessEngine PRIVATE NDEBUG)
#target_compile_definitions(Tim_ChessEngine PRIVATE USE_AVX2 USE_SSE41 USE_SSE3 USE_SSE2 USE_SSE)
target_compile_definitions(Tim_ChessEngine PRIVATE USE_SSE41 USE_SSE3 USE_SSE2 USE_SSE)