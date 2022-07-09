#include "FenParser.h"

#include <sstream>

#include "Utilities.h"

namespace ChessEngine {

    namespace {
        void SetPieces(const PieceInfo &pieceInfo, const BoardTile &tile, Board::Representation &representation) {
            auto[type, team] = pieceInfo;
            if (team == Team::White) {
                representation.own_pieces.Set(tile);
            } else {
                representation.enemy_pieces.Set(tile);
            }

            if (type == PieceType::King) {
                if (team == Team::White) {
                    representation.own_king = tile;
                } else {
                    representation.enemy_king = tile;
                }
            }

            representation.bishop_queens.SetIf(tile, type == PieceType::Queen || type == PieceType::Bishop);
            representation.rook_queens.SetIf(tile, type == PieceType::Queen || type == PieceType::Rook);
            representation.pawns_enPassant.SetIf(tile, type == PieceType::Pawn);
        }

        bool ParsePiecePlacement(const std::string &placement, Board::Representation &representation) {
            // Rank are separated by a '/'.
            // numbers mean consecutive empty spaces.
            // letters correspond to piece types.

            // Kings need to exist for proper behaviour to tile's having a default value of 0.
            bool has_white_king = false;
            bool has_black_king = false;

            std::stringstream stream(placement);
            std::string sub_string;
            int rank = Rank::R8; // Fen strings start from rank 8 going towards rank 1.
            while (getline(stream, sub_string, '/')) { // Iterate over each rank.
                int file = 0;
                for (char token : sub_string) {
                    if (isdigit(token)) { // Skip the empty squares - 1 (due to file++)
                        int empty_squares = token - '0';
                        file += empty_squares - 1;
                    } else {
                        // Get the piece info based on the fen token.
                        PieceInfo piece_info{};
                        if (!CharToPieceInfo(token, piece_info))
                            return false;
                        SetPieces(piece_info, BoardTile(file, rank), representation);

                        auto[type , team] = piece_info;
                        if(type == King){
                            if(team == White)
                                has_white_king = true;
                            else
                                has_black_king = true;
                        }
                    }
                    file++;

                    if (file > File::H + 1) // Check for possible overshoots.
                        return false;
                }

                if (file < File::H + 1) // If a rank isn't fully described.
                    return false;

                rank--;
            }

            // All 8 rank were described (no more or less). Both kings exist
            return (rank == Rank::R1 - 1) && has_white_king && has_black_king;
        }

        bool ParseStartingTeam(const std::string &starting_team_string, Team &starting_team) {
            if (starting_team_string.size() == 1) { // Should be just a single char
                switch (starting_team_string[0]) {
                    case 'w':
                        starting_team = Team::White;
                        return true;
                    case 'b':
                        starting_team = Team::Black;
                        return true;
                    default:
                        return false; // Wrong token.
                }
            } else {
                return false;
            }
        }

        bool ParseCastlingRights(const std::string &rights_string, Board::CastlingRights &castling_rights) {
            // KQkq means every castling move is available , - none.
            // doesn't decline strings with wrong ordering (eg: qKQk)
            bool white_king_side, black_king_side;
            bool white_queen_side, black_queen_side;
            white_king_side = black_king_side = white_queen_side = black_queen_side = false;

            auto string_len = rights_string.length();
            if (string_len == 0 || string_len > 4)
                return false;
            if (rights_string == "-") {
                // Defaults to false.
                castling_rights = Board::CastlingRights();
                return true;
            }

            for (char token : rights_string) {
                PieceInfo piece_info{};
                if (!CharToPieceInfo(token, piece_info))
                    return false;
                auto[type, team] = piece_info;
                bool &king_side = (team == Team::White) ? white_king_side : black_king_side;
                bool &queen_side = (team == Team::White) ? white_queen_side : black_queen_side;

                switch (type) {
                    case PieceType::King:
                        if (!king_side)
                            king_side = true;
                        else
                            return false; // Already set.
                        break;
                    case PieceType::Queen:

                        if (!queen_side)
                            queen_side = true;
                        else
                            return false; // Already set.
                        break;
                    default:
                        return false; // Wrong token.
                }
            }

            castling_rights = {white_queen_side, white_king_side, black_queen_side, black_king_side};
            return true;
        }

        bool ParseMoveCounter(const std::string &counter_string, Board::MoveCounters &move_counters, bool for_half) {
            // create an input stream with your string.
            // number concat with another string still passes!
            int counter = -1;
            std::istringstream iss(counter_string);
            iss >> counter;
            if (iss.fail()) {
                return false;
            }
            if (counter < 0) {
                return false;
            }

            if (for_half) {
                move_counters.half_moves = counter;
            } else {
                move_counters.full_moves = counter;
            }

            return true;
        }

        bool ParseEnPassant(const std::string &enPassant_string, Board::Representation &representation) {
            if (enPassant_string == "-") {
                return true;
            }

            std::tuple<uint8_t, uint8_t> coords = {};
            if (!NotationToCoords(enPassant_string, coords))
                return false;

            auto[file, rank] = coords;
            if (rank != Rank::R3 && rank != Rank::R8)
                return false;
            Rank new_rank = (rank == Rank::R3) ? Rank::R1 : Rank::R8;
            representation.pawns_enPassant.Set(file, new_rank);

            return true;
        }
    }

    bool ParseFenString(const std::string &fen_string, Board::BoardInfo &board_info) {
        Board::Representation representation;
        Board::CastlingRights castling_rights;
        Board::MoveCounters move_counter;
        Team starting_team;

        std::stringstream stream(fen_string);

        int count = 0;
        std::string sub_string;
        while (getline(stream, sub_string, ' ')) {
            // A fen string is split into 6 parts.
            switch (count) {
                case 0:
                    if (!ParsePiecePlacement(sub_string, representation)) return false;
                    break;
                case 1:
                    if (!ParseStartingTeam(sub_string, starting_team)) return false;
                    break;
                case 2:
                    if (!ParseCastlingRights(sub_string, castling_rights)) return false;
                    break;
                case 3:
                    if (!ParseEnPassant(sub_string, representation)) return false;
                    break;
                case 4:
                    if (!ParseMoveCounter(sub_string, move_counter, true)) return false;
                    break;
                case 5:
                    if (!ParseMoveCounter(sub_string, move_counter, false)) return false;
                    break;
                default:
                    return false; // We got more than 6 parts.
            }

            count++;
        }

        if(starting_team == Team::Black) {
            representation.Mirror();
            castling_rights.Mirror();
        }

        if (count == 6) { // We got all 6 parts of the fen format.
            board_info = {representation, castling_rights, move_counter, starting_team};
            return true;
        } else {
            return false;
        }
    }

}