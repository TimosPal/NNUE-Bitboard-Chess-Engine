cmake_minimum_required(VERSION 3.19)
project(ChessEngine2)

set(CMAKE_CXX_STANDARD 20)

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
        MagicNumbers.h PseudoMoves.cpp PseudoMoves.h BoardTile.h)