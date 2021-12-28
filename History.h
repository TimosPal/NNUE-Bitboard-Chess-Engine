#ifndef HISTORY_H
#define HISTORY_H

#define HISTORY_SIZE 256

#include <cstdint>
#include <array>

namespace ChessEngine {
    class History {
    public:
        struct Element {
            uint64_t key; // Zobrist key
            bool progress_made; // If capture or pawn move.
        };

        static History& Instance(){
            static History history;
            return history;
        }

        void AddState(uint8_t index, Element element){
            data[index] = element;
        }

        Element GetState(uint8_t index){
            return data_[index];
        }

    private:
        History();
        std::array<Element, HISTORY_SIZE> data_;
    }
}

#endif