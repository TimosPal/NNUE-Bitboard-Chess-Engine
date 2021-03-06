#include <sstream>
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
        char token = '-';
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
                token = 'n';
                break;
            case Rook:
                token = 'r';
                break;
            case Pawn:
                token = 'p';
                break;
        }

        if(team == Team::White)
            token = (char)toupper(token);
        return  token;
    }

    std::string PieceInfoToAscci(const PieceInfo& piece_info){
        auto[type, team] = piece_info;
        std::string token;
        if(team == Team::White) {
            switch (type) {
                case None:
                    token = "-";
                    break;
                case King:
                    token = "???";
                    break;
                case Queen:
                    token = "???";
                    break;
                case Bishop:
                    token = "???";
                    break;
                case Knight:
                    token = "???";
                    break;
                case Rook:
                    token = "???";
                    break;
                case Pawn:
                    token = "???";
                    break;
            }
        }else{
            switch (type) {
                case None:
                    token = "-";
                    break;
                case King:
                    token = "???";
                    break;
                case Queen:
                    token = "???";
                    break;
                case Bishop:
                    token = "???";
                    break;
                case Knight:
                    token = "???";
                    break;
                case Rook:
                    token = "???";
                    break;
                case Pawn:
                    token = "???";
                    break;
            }
        }

        return  token;
    }

    bool NotationToCoords(const std::string &str, std::tuple<uint8_t, uint8_t>& coords) {
        if (str.length() != 2)
            return false;

        // Is a number between 0-7
        int rank_temp = str[1] - '0' - 1;
        if (rank_temp > 7)
            return false;

        int file_temp = str[0] - 'a';
        if (file_temp > 7)
            return false;

        coords = {file_temp, rank_temp};

        return true;
    }

    bool CoordsToNotation(const std::tuple<uint8_t, uint8_t>& coords, std::string &str){
        auto[file, rank] = coords;
        if (file > 7 || rank > 7) {
            str = "invalid";
            return false;
        }

        char rank_temp = rank + '0' + 1;
        char file_temp = file + 'a';
        str = std::string(1,file_temp) + rank_temp;

        return true;
    }

    std::vector<std::string> Tokenise(const std::string& string){
        std::string word;
        std::istringstream iss(string);
        std::vector<std::string> words;
        while(iss >> word) {
            words.push_back(word);
        }
        return words;
    }

}