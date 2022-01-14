#include "UCI.h"

#include "FenParser.h"
#include "Search.h"

namespace ChessEngine::UCI{

    namespace {

        bool FindWord(const std::vector<std::string>& words, std::string word, int& index){
            int i = 0;
            bool found = false;
            for(const auto& w : words){
                if(w == word){
                    found = true;
                    break;
                }
                i++;
            }
            index = i;
            return found;
        }

        void CommandUCI() {
            static std::string name = "a";
            static std::string author = "b";

            // ID.
            std::cout << "id name " << name << std::endl;
            std::cout << "id author " << author << std::endl;

            // Options.

            // Done.
            std::cout << "uciok" << std::endl;
        }

        Board CommandPosition(const std::vector<std::string> &words) {
            // NOTE: Assume correct format.
            std::string fen = ChessEngine::starting_position_fen;
            auto iter = words.begin() + 1;
            iter++;
            if (*(iter-1) == "fen") {
                fen = "";
                // Concat fen sub parts into 1 string.
                for (int i = 0; i < 6; i++) {
                    fen += *iter + " ";
                    iter++;
                }
            }

            ChessEngine::Board::BoardInfo info = {};
            ChessEngine::ParseFenString(fen, info);
            ChessEngine::Board board(info);

            // Play moves if any.
            if (iter != words.end()) {
                // Skip moves word.
                iter++;
                for (; iter != words.end(); iter++) {
                    Move move(*iter, board.IsFlipped());
                    board.PlayMove(move);
                    board.Mirror();
                }
            }

            return board;
        }

        void CommandGo(const std::vector<std::string> &words, const Board &board) {
            int depth = 8;
            int index;
            if(FindWord(words, "depth", index)){
                depth = atoi(words[index + 1].c_str());
            }

            int eval;
            Move best_move = GetBestMove(board, depth, eval);
            std::cout << "info depth " << depth << " score cp " << eval << std::endl;
            std::cout << "bestmove " << best_move.AlgebraicNotation(board.IsFlipped()) << std::endl;
        }
    }

    void MainLoop(){
        Board board;
        while(true) {
            char command[BUFFER_SIZE];
            std::cin.getline(command, BUFFER_SIZE);

            auto words = Tokenise(command);
            int index;
            if(FindWord(words, "uci", index)){
                CommandUCI();
            }else if(FindWord(words, "isready", index)){
                std::cout << "readyok" << std::endl;
            }else if(FindWord(words, "ucinewgame", index)){
                // Reset TT. TODO
            }else if(FindWord(words, "position", index)){
                board = CommandPosition(words);
            }else if(FindWord(words, "go", index)){
                CommandGo(words, board);
            }else if(FindWord(words, "stop", index)){
                // Stop search timer. TODO
            }else if(FindWord(words, "quit", index)){
                return;
            }
        }
    }
}