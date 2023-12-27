#ifndef ZOBRIST
#define ZOBRIST
#include <random>
#include <vector>

#include "components/types.h"

// implementation with Zobrist Hashing
class Zobrist {
    int size;
    uint_fast64_t hash;  // unsigned long long
    std::mt19937_64 rng{};
    std::vector<std::vector<std::vector<uint_fast64_t>>> zobrist_table;

   public:
    Zobrist(int size);
    void modifyBoard(int x, int y, Color color);
    uint_fast64_t get_hash();
};
#endif
