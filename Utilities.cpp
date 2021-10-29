#include "Utilities.h"

namespace ChessEngine {

    bool CharToPieceInfo(char token, PieceInfo &piece_info) {
        PieceType type;
        switch (tolower(token)) {
            case 'p':
                type = PieceType::Pawn;
                break;
            case 'r':
                type = PieceType::Rook;
                break;
            case 'n':
                type = PieceType::Knight;
                break;
            case 'k':
                type = PieceType::King;
                break;
            case 'q':
                type = PieceType::Queen;
                break;
            case 'b':
                type = PieceType::Bishop;
                break;
            default:
                return false; // Wrong token.
        }

        // Lowercase is Black , whereas uppercase is White.
        Team team = islower(token) ? Team::Black : Team::White;
        piece_info = {type, team};
        return true;
    }

    char PieceInfoToChar(const PieceInfo& piece_info){
        auto[type, team] = piece_info;
        char token;
        switch (type) {
            case None:
                token = '-';
                break;
            case King:
                token = 'k';
                break;
            case Queen:
                token = 'q';
                break;
            case Bishop:
                token = 'b';
                break;
            case Knight:
                token = 'k';
                break;
            case Rook:
                token = 'r';
                break;
            case Pawn:
                token = 'p';
                break;
        }

        if(team == Team::White)
            token = toupper(token);
        return  token;
    }

    bool NotationToCoords(const std::string &str, std::tuple<uint8_t, uint8_t>& coords) {
        if (str.length() != 2)
            return false;

        // Is a number between 0-7
        int rank_temp = str[1] - '0' - 1;
        if (rank_temp < 0 || rank_temp > 7)
            return false;

        int file_temp = str[0] - 'a';
        if (file_temp < 0 || file_temp > 7)
            return false;

        coords = {file_temp, rank_temp};

        return true;
    }

    bool CoordsToNotation(const std::tuple<uint8_t, uint8_t>& coords, std::string &str){
        auto[file, rank] = coords;
        if (file < 0 || file > 7 || rank < 0 || rank > 7)
            return false;

        char rank_temp = rank + '0' + 1;
        char file_temp = file + 'a';
        str = std::string(1,file_temp) + rank_temp;

        return true;
    }

}