#include <iostream>

#include "AttackTables.h"
#include "FenParser.h"

int main() {
    {
    PROFILE_SCOPE("Program");
    ChessEngine::AttackTables::InitMoveTables();

    //std::string fen = "7r/6N1/q1P2KQb/4PP2/5r2/2b5/8/2k5 w - - 0 1";
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    ChessEngine::Board::BoardInfo info = {};
    IF_ERROR(!ChessEngine::ParseFenString(fen, info), "Invalid fen string.")
    ChessEngine::Board board(info);

    /*
    auto moves = board.GetLegalMoves();
    std::cout << "Moves : " << moves.size() << std::endl;
    for(auto move : moves){
        ChessEngine::Board temp =  board;
        //std::cout << std::endl << "Starting" << std::endl << std::endl;
        //temp.Draw();
        std::cout << std::endl << move << std::endl << std::endl;
        temp.PlayMove(move);
        temp.Mirror();
        temp.Draw();
    }
    */

    auto[nodes, captures, checks] = ChessEngine::Board::Perft(3, board);
    std::cout << "Nodes : " << nodes << std::endl;
    std::cout << "Captures : " << captures << std::endl;
    std::cout << "Checks : " << checks << std::endl;
    }
    ChessEngine::Timer::Print();

    return 0;
}
