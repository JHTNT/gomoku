#ifndef EVAL
#define EVAL
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "components/types.h"

class Evaluator {
    const int size;
    std::vector<std::vector<short>> board;
    std::vector<std::vector<short>> black_score;
    std::vector<std::vector<short>> white_score;
    // pattern_cache[color][direction][row][col];
    std::vector<std::vector<std::vector<std::vector<short>>>> pattern_cache;
    // vertical, horizontal, diag (\), diag (/)
    const Points all_directions = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
    std::vector<std::pair<int, Color>> history;

   public:
    Evaluator(int n);
    int evaluate(Color color);
    int directionToIndex(int x, int y);
    void putStone(int x, int y, Color color);
    void takeStone(int x, int y, Color color);
    void updatePoint(int x, int y, Color color);
    void updatePointPattern(int x, int y, Color color, Points direction);
    bool isPointInLine(int point, std::vector<int> arr);
    std::unordered_map<int, std::unordered_set<int>> getPoints(Color color, int depth);
    std::unordered_set<int> getMoves(Color color, int depth);
};
#endif
