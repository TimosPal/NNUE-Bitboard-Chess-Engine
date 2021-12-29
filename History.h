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

        void AddState(uint8_t ply, Element element){
            data_[ply] = element;
        }

        Element GetState(uint8_t ply){
            return data_[ply];
        }

    private:
        History() = default;
        std::array<Element, HISTORY_SIZE> data_;
    };
}

#endif