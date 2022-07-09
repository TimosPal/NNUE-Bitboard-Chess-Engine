#ifndef UCI_H
#define UCI_H

#include <string>
#include <representation/Board.h>

namespace ChessEngine::UCI{
    // Rest of the functions are helpers for the main loop and are kept
    // encapsulated within the cpp file.
    void MainLoop();
}

#endif
