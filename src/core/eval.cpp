#include "core/eval.h"

#include <cmath>
#include <iostream>
#include <numeric>
#include <ranges>

#include "core/pattern.h"

using namespace std;

Evaluator::Evaluator(int n)
    : size{n},
      board{vector<vector<short>>(n, vector<short>(n, -1))},
      black_score{vector<vector<short>>(n, vector<short>(n, 0))},
      white_score{vector<vector<short>>(n, vector<short>(n, 0))},
      pattern_cache{vector<vector<vector<vector<short>>>>(
          2, vector<vector<vector<short>>>(4, vector<vector<short>>(n, vector<short>(n, 0))))} {}

int Evaluator::evaluate(Color color) {
    int black = 0, white = 0;
    for (auto i : black_score) {
        for (int score : i) black += score;
    }
    for (auto i : white_score) {
        for (int score : i) white += score;
    }
    return color == BLACK ? black - white : white - black;
}

int Evaluator::directionToIndex(int x, int y) {
    if (x == 0) return 0;  // vertical
    if (y == 0) return 1;  // horizontal
    if (x == y) return 2;  // diag (\)
    if (x != y) return 3;  // diag (/)
    return -1;
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
    updatePoint(x, y);
    this->history.push_back({x * this->size + y, color});
}

void Evaluator::takeStone(int x, int y, Color color) {
    this->board[x][y] = -1;
    updatePoint(x, y);
    this->history.pop_back();
}

void Evaluator::updatePoint(int x, int y) {
    updatePointPattern(x, y, BLACK, Points{});
    updatePointPattern(x, y, WHITE, Points{});

    for (auto d : all_directions) {
        for (int sign : {1, -1}) {  // 1 for positive direction, -1 for negative direction
            for (int step = 1; step <= 5; step++) {
                bool reach_edge = false;
                for (Color c : {BLACK, WHITE}) {
                    int new_x = x + step * sign * d.first, new_y = y + step * sign * d.second;
                    if (new_x < 0 || new_x >= size || new_y < 0 || new_y >= size) {
                        reach_edge = true;
                        break;
                    } else if (board[new_x][new_y] == ~c) {
                        continue;  // if find opponent stone, evaluate opponent color
                    } else if (board[new_x][new_y] == -1) {
                        this->updatePointPattern(new_x, new_y, c,
                                                 Points{{d.first * sign, d.second * sign}});
                    }
                }
                if (reach_edge) break;
            }
        }
    }
}

void Evaluator::updatePointPattern(int x, int y, Color color, Points directions) {
    if (board[x][y] != Color::EMPTY) return;
    if (directions.size() == 0) directions = all_directions;

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

    // adjust score for edge
    if (x == 0 || y == 0 || x == size - 1 || y == size - 1) {
        score *= 0.9;
    } else if (x == 1 || y == 1 || x == size - 2 || y == size - 2) {
        score *= 0.95;
    }

    if (color == BLACK) {
        this->black_score[x][y] = score;
    } else {
        this->white_score[x][y] = score;
    }
}

bool Evaluator::isPointInLine(int point, vector<int> arr) {
    int inline_distance = 5;  // configurable
    int x1 = floor(point / this->size), y1 = point % this->size;
    for (auto x : arr) {
        int x2 = floor(x / this->size), y2 = x % this->size;
        if ((x1 == x2 && abs(y1 - y2) < inline_distance) ||
            (y1 == y2 && abs(x1 - x2) < inline_distance) ||
            (abs(x1 - x2) == abs(y1 - y2) && abs(x1 - x2) < inline_distance))
            return true;
    }
    return false;
}

int Evaluator::getPatternCountOfPoint(int x, int y, Color color) {
    int cnt = 0;
    if (color == Color::EMPTY) {
        for (int d = 0; d < 4; d++) {
            if (this->pattern_cache[0][d][x][y] > Pattern::NONE) cnt++;
            if (this->pattern_cache[1][d][x][y] > Pattern::NONE) cnt++;
        }
    } else {
        for (int d = 0; d < 4; d++) {
            if (this->pattern_cache[color][d][x][y] > Pattern::NONE) cnt++;
        }
    }

    return cnt;
}

unordered_map<int, unordered_set<int>> Evaluator::getPoints(Color color, int depth, bool vct,
                                                            bool vcf) {
    Color first = depth % 2 ? ~color : color;  // if depth is even, set first as self color
    unordered_map<int, unordered_set<int>> points;
    for (int pattern : {FIVE, BLOCK_FIVE, FOUR, FOUR_FOUR, FOUR_THREE, THREE_THREE, BLOCK_FOUR,
                        THREE, BLOCK_THREE, TWO_TWO, TWO, NONE}) {
        points[pattern] = unordered_set<int>();
    }

    vector<int> last_points;
    // vector<pair<int, Color>> processed;
    for (auto it = history.end() - min<int>(history.size(), 4); it != history.end(); it++) {
        last_points.push_back(it->first);
    }

    for (Color c : {color, ~color}) {
        for (int x = 0; x < this->size; x++) {
            for (int y = 0; y < this->size; y++) {
                int four_cnt = 0, block_four_cnt = 0, three_cnt = 0;
                for (int direction = 0; direction < 4; direction++) {
                    if (this->board[x][y] != -1) continue;  // already put stone
                    Pattern p = Pattern(this->pattern_cache[c][direction][x][y]);
                    if (p == Pattern::NONE) continue;

                    int point = x * this->size + y;

                    if (vcf) {
                        if (c == first && !isFour(p) && !isFive(p)) {
                            continue;
                        } else if (c == ~first && isFive(p)) {
                            continue;
                        }
                    }

                    if (vct) {
                        // only consider self turn
                        if (depth % 2 == 0) {
                            if (depth == 0 && c != first) continue;  // only attack
                            if (p != Pattern::THREE && !isFour(p) && !isFive(p)) continue;
                            if (p == Pattern::THREE && c != first) continue;
                            if (depth == 0 && c != first) continue;
                            if (depth > 0 && (p == Pattern::THREE || p == Pattern::BLOCK_FOUR) &&
                                getPatternCountOfPoint(x, y, c) == 1)
                                continue;
                        } else {  // only consider defence
                            if (p != Pattern::THREE && !isFour(p) && !isFive(p)) continue;
                            if (p == Pattern::THREE && c == ~first) continue;
                            if (depth > 1) {
                                if (p == Pattern::BLOCK_FOUR &&
                                    getPatternCountOfPoint(x, y, Color::EMPTY) == 1)
                                    continue;
                                if (p == Pattern::BLOCK_FOUR && !isPointInLine(point, last_points))
                                    continue;
                            }
                        }
                    }

                    if (vcf && !isFour(p) && !isFive(p)) continue;

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

vector<int> Evaluator::getMoves(Color color, int depth, bool vct, bool vcf) {
    auto points = getPoints(color, depth, vct, vcf);

    auto fives = points[Pattern::FIVE];
    auto block_fives = points[Pattern::BLOCK_FIVE];
    if (fives.size() || block_fives.size()) {
        fives.merge(block_fives);
        return {fives.begin(), fives.end()};
    }

    auto fours = points[Pattern::FOUR];
    auto block_fours = points[Pattern::BLOCK_FOUR];
    if (vcf || fours.size()) {
        fours.merge(block_fours);
        return {fours.begin(), fours.end()};
    }
    auto four_fours = points[Pattern::FOUR_FOUR];
    if (four_fours.size()) {
        four_fours.merge(block_fours);
        return {four_fours.begin(), four_fours.end()};
    }

    auto four_threes = points[Pattern::FOUR_THREE];
    auto threes = points[Pattern::THREE];
    if (four_threes.size()) {
        four_threes.merge(block_fours);
        four_threes.merge(threes);
        return {four_threes.begin(), four_threes.end()};
    }
    auto three_threes = points[Pattern::THREE_THREE];
    if (three_threes.size()) {
        three_threes.merge(block_fours);
        three_threes.merge(threes);
        return {three_threes.begin(), three_threes.end()};
    }

    unordered_set<int> others;
    vector<int> res;
    auto it = block_fours.begin();
    while (others.size() < 20 && it != block_fours.end()) {
        if (others.find(*it) == others.end()) {
            others.insert(*it);
            res.push_back(*it);
        }
        it++;
    }
    if (others.size() >= 20) return res;

    it = threes.begin();
    while (others.size() < 20 && it != threes.end()) {
        if (others.find(*it) == others.end()) {
            others.insert(*it);
            res.push_back(*it);
        }
        it++;
    }
    if (others.size() >= 20) return res;

    auto block_threes = points[Pattern::BLOCK_THREE];
    it = block_threes.begin();
    while (others.size() < 20 && it != block_threes.end()) {
        if (others.find(*it) == others.end()) {
            others.insert(*it);
            res.push_back(*it);
        }
        it++;
    }
    if (others.size() >= 20) return res;

    auto two_twos = points[Pattern::TWO_TWO];
    it = two_twos.begin();
    while (others.size() < 20 && it != two_twos.end()) {
        if (others.find(*it) == others.end()) {
            others.insert(*it);
            res.push_back(*it);
        }
        it++;
    }
    if (others.size() >= 20) return res;

    auto two = points[Pattern::TWO];
    it = two.begin();
    while (others.size() < 25 && it != two.end()) {
        if (others.find(*it) == others.end()) {
            others.insert(*it);
            res.push_back(*it);
        }
        it++;
    }

    return res;
}
