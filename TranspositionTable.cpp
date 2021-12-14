#include <iostream>
#include "TranspositionTable.h"

namespace ChessEngine{

    TTEntry::TTEntry(uint8_t depth, int evaluation, NodeType type, const MoveList& pv){
        this->evaluation = evaluation;
        this->depth = depth;
        this->type = type;
        this->pv = pv;
    }

    void TranspositionTable::AddEntry(uint64_t zobrist_key, const TTEntry& entry){
        auto it = table_.find(zobrist_key);
        if(it != table_.end()) {
            if(entry.depth >= it->second.depth) {
                it->second = entry;
            }
        }else {
            table_.insert({zobrist_key, entry});
        }
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