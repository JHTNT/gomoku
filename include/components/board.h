#ifndef BOARD
#define BOARD
#include <vector>

#include "core/eval.h"

class Board {
    const int size;
    Color next_color;
    Evaluator evaluator;
    std::vector<std::vector<short>> board;

   public:
    Board(int n);
    bool isGameOver();
    int evaluate(Color color);
    void putStone(Point move, Color color);
    void takeStone(Point move, Color color);
    Points getValuableMoves(Color color, int depth);

    int get_pixel(int i, int j) { return this->board[i][j]; };
    int get_size() { return this->size; }
};
#endif
