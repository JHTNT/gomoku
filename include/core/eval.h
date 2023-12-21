#ifndef EVAL
#define EVAL
#include <vector>

#include "components/board.h"
#include "components/types.h"

class Evaluator {
    const int size;
    Board* b;
    std::vector<std::vector<short>> board;
    std::vector<std::vector<short>> black_score;
    std::vector<std::vector<short>> white_score;
    // pattern_cache[color][direction][row][col];
    std::vector<std::vector<std::vector<std::vector<short>>>> pattern_cache;
    // vertical, horizontal, diag (\), diag (/)
    const Points all_directions = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};

   public:
    Evaluator(int n, Board board);
    int evaluate(Color color);
    int directionToIndex(int i, int j);
    void updatePoint(int i, int j, Color color);
    void updatePointPattern(int i, int j, Color color, Points direction);
};
#endif
