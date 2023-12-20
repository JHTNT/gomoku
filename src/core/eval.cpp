#include "core/eval.h"

#include <numeric>
#include <ranges>

using namespace std;

Evaluator::Evaluator(int n, Board b_)
    : size{n},
      b{&b_},
      board{vector<vector<short>>(n, vector<short>(n, -1))},
      black_score{vector<vector<short>>(n, vector<short>(n, -1))},
      white_score{vector<vector<short>>(n, vector<short>(n, -1))},
      pattern_cache{vector<vector<vector<vector<short>>>>(
          2, vector<vector<vector<short>>>(4, vector<vector<short>>(n, vector<short>(n, 0))))} {}

int Evaluator::evaluate(Color color) {
    int black = accumulate(black_score.begin(), black_score.end(), 0);
    int white = accumulate(white_score.begin(), white_score.end(), 0);
    return color == Black ? black - white : white - black;
}

int directionToIndex(int x, int y) {
    if (x == 0) return 0;  // vertical
    if (y == 0) return 1;  // horizontal
    if (x == y) return 2;  // diag (\)
    if (x != y) return 3;  // diag (/)
}

// get the score before putting this stone
int getPatternScore(Pattern pattern) {
    switch (pattern) {
        case Pattern::FIVE:
            return 100000;  // FOUR
        case Pattern::BLOCK_FIVE:
            return 1500;  // BLOCK_FOUR
        case Pattern::FOUR:
        case Pattern::FOUR_FOUR:
        case Pattern::FOUR_THREE:
            return 1000;  // THREE
        case Pattern::BLOCK_FOUR:
            return 150;  // BLOCK_THREE
        case Pattern::THREE:
        case Pattern::THREE_THREE:
            return 100;  // TWO
        case Pattern::TWO_TWO:
            return 20;
        case Pattern::BLOCK_THREE:
            return 15;  // BLOCK_TWO
        case Pattern::TWO:
            return 10;  // ONE
        default:
            return 0;
    }
}

void Evaluator::updatePoint(int x, int y, Color color) {
    // clear scores of this point
    for (int d = 0; d < 4; d++) {
        this->pattern_cache[color][d][x][y] = 0;
        this->pattern_cache[~color][d][x][y] = 0;
    }
    this->black_score[x][y] = 0;
    this->white_score[x][y] = 0;
    this->board[x][y] = color;
}

void Evaluator::updatePointPattern(int x, int y, Color color, Points directions = Points{}) {
    if (board[x][y] != -1) return;
    if (!directions.size()) directions = all_directions;

    this->board[x][y] = color;  // put temporarily stone

    for (auto d : directions) {
        pattern_cache[color][directionToIndex(d.first, d.second)][x][y] = Pattern::NONE;
    }

    int score = 0, block_four_cnt = 0, three_cnt = 0, two_cnt = 0;
    for (int d = 0; d < 4; d++) {
        Pattern p = Pattern(pattern_cache[color][d][x][y]);
        if (p > Pattern::NONE) {
            score += getPatternScore(p);
            if (p == Pattern::BLOCK_FOUR) block_four_cnt++;
            if (p == Pattern::THREE) three_cnt++;
            if (p == Pattern::TWO) two_cnt++;
        }
    }

    for (auto d : directions) {
        Pattern p = getPattern(board, x, y, d.first, d.second, color);
        if (p == Pattern::NONE) continue;

        pattern_cache[color][directionToIndex(d.first, d.second)][x][y] = p;
        if (p == Pattern::BLOCK_FOUR) block_four_cnt++;
        if (p == Pattern::THREE) three_cnt++;
        if (p == Pattern::TWO) two_cnt++;
        // combine multiple patterns to a complex pattern
        if (block_four_cnt >= 2) {
            p = Pattern::FOUR_FOUR;
        } else if (block_four_cnt && three_cnt) {
            p = Pattern::FOUR_THREE;
        } else if (three_cnt >= 2) {
            p = Pattern::THREE_THREE;
        } else if (two_cnt >= 2) {
            p = Pattern::TWO_TWO;
        }
        score += getPatternScore(p);
    }

    this->board[x][y] = -1;  // remove temporarily stone
    if (color == Black) {
        this->black_score[x][y] = score;
    } else {
        this->white_score[x][y] = score;
    }
}
