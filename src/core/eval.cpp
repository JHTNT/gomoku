#include "core/eval.h"

#include <cmath>
#include <numeric>
#include <ranges>

#include "core/pattern.h"

using namespace std;

Evaluator::Evaluator(int n)
    : size{n},
      board{vector<vector<short>>(n, vector<short>(n, -1))},
      black_score{vector<vector<short>>(n, vector<short>(n, -1))},
      white_score{vector<vector<short>>(n, vector<short>(n, -1))},
      pattern_cache{vector<vector<vector<vector<short>>>>(
          2, vector<vector<vector<short>>>(4, vector<vector<short>>(n, vector<short>(n, 0))))} {}

int Evaluator::evaluate(Color color) {
    int black = accumulate(black_score.begin(), black_score.end(), 0);
    int white = accumulate(white_score.begin(), white_score.end(), 0);
    return color == BLACK ? black - white : white - black;
}

int directionToIndex(int x, int y) {
    if (x == 0) return 0;  // vertical
    if (y == 0) return 1;  // horizontal
    if (x == y) return 2;  // diag (\)
    if (x != y) return 3;  // diag (/)
}

void Evaluator::putStone(int x, int y, Color color) {
    // clear scores of this point
    for (int d = 0; d < 4; d++) {
        this->pattern_cache[color][d][x][y] = 0;
        this->pattern_cache[~color][d][x][y] = 0;
    }
    this->black_score[x][y] = 0;
    this->white_score[x][y] = 0;
    this->board[x][y] = color;
    updatePoint(x, y, color);
    this->history.push_back({x * this->size + y, color});
}
void Evaluator::takeStone(int x, int y, Color color) {
    this->board[x][y] = -1;
    updatePoint(x, y, color);
    this->history.pop_back();
}

void Evaluator::updatePoint(int x, int y, Color color) {
    updatePointPattern(x, y, BLACK);
    updatePointPattern(x, y, WHITE);

    for (auto d : all_directions) {
        for (int sign : {1, -1}) {  // 1 for positive direction, -1 for negative direction
            for (int step = 1; step <= 5; step++) {
                bool reach_edge = false;
                for (Color c : {BLACK, WHITE}) {
                    int new_x = x + step * sign * d.first, new_y = y + step * sign * d.second;
                    if (new_x < 0 || new_x >= size || new_y < 0 || new_y >= size) {
                        reach_edge = true;
                        break;
                    } else if (board[new_x][new_y] == ~color) {
                        continue;  // if find opponent stone, evaluate opponent color
                    } else if (board[new_x][new_y] == -1) {
                        this->updatePointPattern(new_x, new_y, color,
                                                 Points{{d.first * sign, d.second * sign}});
                    }
                }
                if (reach_edge) break;
            }
        }
    }
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
    if (color == BLACK) {
        this->black_score[x][y] = score;
    } else {
        this->white_score[x][y] = score;
    }
}

bool Evaluator::isPointInLine(int point, vector<int> arr) {
    int inline_distance = 5;  // configurable
    for (auto x : arr) {
        int x1 = floor(point / this->size), y1 = point % this->size;
        int x2 = floor(x / this->size), y2 = x % this->size;
        if ((x1 == x2 && abs(y1 - y1) < inline_distance) ||
            (y1 == y1 && abs(x1 - x2) < inline_distance))
            return true;
    }
    return false;
}

unordered_map<int, unordered_set<int>> Evaluator::getPoints(Color color, int depth) {
    unordered_map<int, unordered_set<int>> points;
    for (int pattern : {FIVE, BLOCK_FIVE, FOUR, FOUR_FOUR, FOUR_THREE, THREE_THREE, BLOCK_FOUR,
                        THREE, BLOCK_THREE, TWO_TWO, TWO, NONE}) {
        points[pattern] = unordered_set<int>();
    }

    vector<int> last_points;
    // vector<pair<int, Color>> processed;
    for (int i = this->history.size() - 1; i >= history.size() - 4; i--) {
        last_points.push_back(this->history[i].first);
    }

    for (Color c : {color, ~color}) {
        for (int x = 0; x < this->size; x++) {
            for (int y = 0; y < this->size; y++) {
                int four_cnt = 0, block_four_cnt = 0, three_cnt = 0;
                for (int direction = 0; direction < 4; direction++) {
                    int point = x * this->size + y;
                    Pattern p = Pattern(this->pattern_cache[c][direction][x][y]);

                    // ignore pattern value less than THREE when depth >= 3
                    if (depth > 2 &&
                        (p == Pattern::TWO || p == Pattern::TWO_TWO || p == Pattern::BLOCK_THREE) &&
                        !isPointInLine(point, last_points)) {
                        continue;
                    }

                    points[p].insert(point);
                    if (p == Pattern::FOUR)
                        four_cnt++;
                    else if (p == Pattern::BLOCK_FOUR)
                        block_four_cnt++;
                    else if (p == Pattern::THREE)
                        three_cnt++;

                    // union pattern
                    if (four_cnt >= 2)
                        points[Pattern::FOUR_FOUR].insert(point);
                    else if (block_four_cnt && three_cnt)
                        points[Pattern::FOUR_THREE].insert(point);
                    else if (three_cnt >= 2)
                        points[Pattern::THREE_THREE].insert(point);
                }
            }
        }
    }
    return points;
}

unordered_set<int> Evaluator::getMoves(Color color, int depth) {
    auto points = getPoints(color, depth);

    auto fours = points[Pattern::FOUR];
    auto block_fours = points[Pattern::BLOCK_FOUR];
    if (fours.size()) {
        fours.merge(block_fours);
        return fours;
    }
    auto four_fours = points[Pattern::FOUR_FOUR];
    if (four_fours.size()) {
        four_fours.merge(block_fours);
        return four_fours;
    }

    auto four_threes = points[Pattern::FOUR_THREE];
    auto threes = points[Pattern::THREE];
    if (four_threes.size()) {
        four_threes.merge(threes);
        return four_threes;
    }
    auto three_threes = points[Pattern::THREE_THREE];
    if (three_threes.size()) {
        three_threes.merge(threes);
        return three_threes;
    }

    auto others = block_fours;
    if (others.size() >= 20) return others;
    others.merge(threes);
    if (others.size() >= 20) return others;
    others.merge(points[Pattern::BLOCK_THREE]);
    if (others.size() >= 20) return others;
    others.merge(points[Pattern::TWO_TWO]);
    if (others.size() >= 20) return others;
    others.merge(points[Pattern::TWO]);
    return others;
}
