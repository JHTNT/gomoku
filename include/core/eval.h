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
    int directionToIndex(int x, int y);
    void putStone(int x, int y, Color color);
    void takeStone(int x, int y, Color color);
    void updatePoint(int x, int y, Color color);
    void updatePointPattern(int x, int y, Color color, Points direction);
};
#endif
