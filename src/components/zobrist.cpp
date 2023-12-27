#include "components/zobrist.h"

using namespace std;

Zobrist::Zobrist(int n)
    : size{n},
      hash{rng()},
      zobrist_table(2, vector<vector<uint_fast64_t>>(n, vector<uint_fast64_t>(n))) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            zobrist_table[0][i][j] = rng();
            zobrist_table[1][i][j] = rng();
        }
    }
}

void Zobrist::modifyBoard(int x, int y, Color color) {
    this->hash ^= this->zobrist_table[color][x][y];
}

uint_fast64_t Zobrist::get_hash() { return this->hash; }
