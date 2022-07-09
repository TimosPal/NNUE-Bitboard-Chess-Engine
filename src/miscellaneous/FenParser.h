#ifndef FEN_PARSER_H
#define FEN_PARSER_H

#include <string>

#include <representation/Board.h>

namespace ChessEngine {
    /* Various helper functions are implemented within the .cpp file */
    bool ParseFenString(const std::string &fen_string, Board::BoardInfo &board_info);
}

#endif