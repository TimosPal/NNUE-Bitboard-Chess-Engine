cmake_minimum_required(VERSION 3.16.3)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 20)

include_directories(./src
        ./dependencies)

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
        src/representation/Bitboard.h
        src/representation/Bitboard.cpp
        src/representation/Board.cpp
        src/representation/Board.h
        src/moves/Move.h
        src/miscellaneous/FenParser.h
        src/miscellaneous/FenParser.cpp
        src/miscellaneous/Utilities.cpp
        src/miscellaneous/Utilities.h
        src/representation/AttackTables.cpp
        src/representation/AttackTables.h
        src/miscellaneous/MagicNumbers.h
        src/moves/PseudoMoves.cpp
        src/moves/PseudoMoves.h
        src/representation/BoardTile.h
        src/miscellaneous/Timer.cpp
        src/miscellaneous/Timer.h
        src/search/NNUE.h
        src/search/NNUE.cpp src/search/Search.h
        src/search/Search.cpp
        src/search/ZobristKey.h
        src/search/ZobristKey.cpp
        src/search/TranspositionTable.h
        src/search/TranspositionTable.cpp
        src/miscellaneous/UCI.h
        src/moves/Move.cpp
        src/miscellaneous/UCI.cpp
        src/representation/History.h
        src/search/MCTS.h
        src/search/MCTS.cpp
        dependencies/nnue-probe/src/misc.cpp
        dependencies/nnue-probe/src/misc.h
        dependencies/nnue-probe/src/nnue.cpp
        dependencies/nnue-probe/src/nnue.h)

message("-----------------------------------")
message("Profiler enabled.")
target_compile_definitions(${EXE_NAME} PRIVATE PROFILER)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Disable assertions in release mode.
    message("Assertions disabled.")
    target_compile_definitions(${EXE_NAME} PRIVATE NDEBUG)
endif ()

target_compile_definitions(${EXE_NAME} PRIVATE ${INTRINSICS_DEFINES})
target_compile_definitions(${EXE_NAME} PRIVATE "DLL_EXPORT")

message("Linking : ${CMAKE_CXX_FLAGS}")
message("Release flags : ${CMAKE_CXX_FLAGS_RELEASE}")
message("-----------------------------------")