#include <iostream>

#include "AttackTables.h"
#include "FenParser.h"

int main() {
    {
    PROFILE_SCOPE("Program");
    ChessEngine::AttackTables::InitMoveTables();

    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    ChessEngine::Board::BoardInfo info = {};
    IF_ERROR(!ChessEngine::ParseFenString(fen, info), "Invalid fen string.")
    ChessEngine::Board board(info);

        for (int i = 0; i < 10000; i++) {
            ChessEngine::Board temp = board;
            for (int j = 0; j < 500; j++) {
                auto moves = temp.GetLegalMoves();
                ChessEngine::GameResult result;
                if((result = temp.Result(moves)) != ChessEngine::GameResult::Playing){
                    switch (result) {
                        case ChessEngine::GameResult::WhiteWon:
                            std::cout << "White win" << std::endl;
                            break;
                        case ChessEngine::GameResult::BlackWon:
                            std::cout << "Black won" << std::endl;
                            break;
                        case ChessEngine::GameResult::Draw:
                            std::cout << "Draw" << std::endl;
                            break;
                    }
                    if(temp.IsFlipped()){
                        temp.Mirror();
                    }
                    std::cout << temp.Fen() << std::endl;
                    break;
                }
                temp.PlayMove(moves[rand() % moves.size()]);
                temp.Mirror();
            }
        }

    //int nodes = ChessEngine::Board::Perft(5, board);
    //std::cout << "Nodes : " << nodes << std::endl;
    }
    ChessEngine::Timer::Print();

    return 0;
}
