cmake_minimum_required(VERSION 3.16.3)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 20)

# Ensure a build type is set.
if(NOT CMAKE_BUILD_TYPE)
    message("Build type automatically set to [Release].")
    set(CMAKE_BUILD_TYPE Release)
endif()

# Exe name.
set(EXE_NAME MyEngine)
# Link with std lib.
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
# Optimize code in release mode.
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

# - - - - - - - - - - - - - - - - - - - - - - - - - - #
# Enable only the instructions supported by your cpu. #
# - - - - - - - - - - - - - - - - - - - - - - - - - - #

#list(APPEND INTRINSICS_DEFINES "USE_AVX2")
list(APPEND INTRINSICS_DEFINES "USE_SSE41")
list(APPEND INTRINSICS_DEFINES "USE_SSE3")
list(APPEND INTRINSICS_DEFINES "USE_SSE2")
list(APPEND INTRINSICS_DEFINES "USE_SSE")

# - - - - - - - - - - - - - - - - - - - - - - - - - - #
# --------------------------------------------------- #
# - - - - - - - - - - - - - - - - - - - - - - - - - - #

# Link based on definitions.
if("USE_AVX2" IN_LIST INTRINSICS_DEFINES)
    list(APPEND INTRINSICS "-mavx2")
endif()
if("USE_SSE41" IN_LIST INTRINSICS_DEFINES)
    list(APPEND INTRINSICS "-msse4.1")
endif()
if("USE_SSE3" IN_LIST INTRINSICS_DEFINES)
    list(APPEND INTRINSICS "-msse3")
endif()
if("USE_SSE2" IN_LIST INTRINSICS_DEFINES)
    list(APPEND INTRINSICS "-msse2")
endif()
if("USE_SSE" IN_LIST INTRINSICS_DEFINES)
    list(APPEND INTRINSICS "-msse")
endif()
list(JOIN INTRINSICS " " INTRINSICS)

# Enables platform specific instructions.
set(CMAKE_CXX_FLAGS ${INTRINSICS})

add_executable(${EXE_NAME}
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

message("-----------------------------------")
message("Profiler enabled.")
target_compile_definitions(${EXE_NAME} PRIVATE PROFILER)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Disable assertions in release mode.
    message("Assertions disabled.")
    target_compile_definitions(${EXE_NAME} PRIVATE NDEBUG)
endif ()

target_compile_definitions(${EXE_NAME} PRIVATE ${INTRINSICS_DEFINES})

message("Linking : ${CMAKE_CXX_FLAGS}")
message("Release flags : ${CMAKE_CXX_FLAGS_RELEASE}")
message("-----------------------------------")