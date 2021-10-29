#include "Utilities.h"

namespace ChessEngine {

    bool AlgebraicNotationToCoord(const std::string &str, BoardTile::Coordinates &coords) {
        if (str.length() != 2)
            return false;

        // Is a number between 0-7
        int rank_temp = str[1] - '0' - 1;
        if (rank_temp < 0 || rank_temp > 7)
            return false;

        int file_temp = str[0] - 'a';
        if (file_temp < 0 || file_temp > 7)
            return false;

        // Safe cast to Enums.
        coords = {file_temp, rank_temp};

        return true;
    }

}