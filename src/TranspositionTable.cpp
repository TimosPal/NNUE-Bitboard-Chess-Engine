#include <iostream>
#include "TranspositionTable.h"

namespace ChessEngine{

    TTEntry::TTEntry(uint8_t depth, int evaluation, NodeType type, Move best_move){
        this->evaluation = evaluation;
        this->depth = depth;
        this->type = type;
        this->best_move = best_move;
    }

    void TranspositionTable::AddEntry(uint64_t zobrist_key, const TTEntry& entry){
        table_[zobrist_key] = entry;
    }

    bool TranspositionTable::GetEntry(uint64_t zobrist_key, TTEntry& result) const {
        auto it = table_.find(zobrist_key);
        if(it != table_.end()) {
            result = it->second;
            return true;
        }
        return false;
    }

    int TranspositionTable::GetSize() const {
        return table_.size();
    }

}