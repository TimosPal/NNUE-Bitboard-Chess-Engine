#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include <unordered_map>

#include <moves/Move.h>

namespace ChessEngine{

    class TranspositionTable{
    public:
        enum class NodeType{
            Alpha, Beta, Exact
        };

        struct TTEntry{
            NodeType type; // Determines if we check a,b or just return.
            int evaluation; // Position evaluation.
            uint8_t depth; // depth of search's iteration.
            Move best_move; // Picked move on said search's node.

            TTEntry(uint8_t depth, int evaluation, NodeType, Move best_move = Move());
            TTEntry() = default;
        };

        void Clear() { table_.clear(); }
        void Reserve(int size) { table_.reserve(size); }
        void AddEntry(uint64_t zobrist_key, const TTEntry& entry);
        bool GetEntry(uint64_t zobrist_key, TTEntry& result) const;
        int GetSize() const;

    private:
        // <zobrist key, entry>
        std::unordered_map<uint64_t, TTEntry> table_;
    };

}

#endif