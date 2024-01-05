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
        pattern_cache[color][directionToIndex(d.first, d.second)][x][y] = Pattern::DEAD;
    }

    int score = 0;
    int p_cnt[Pattern::FIVE + 1] = {0};
    for (int d = 0; d < 4; d++) {
        Pattern p = Pattern(pattern_cache[color][d][x][y]);
        if (p > Pattern::DEAD) p_cnt[p]++;
    }

    for (auto d : directions) {
        Pattern p = getPattern(board, x, y, d.first, d.second, color);
        pattern_cache[color][directionToIndex(d.first, d.second)][x][y] = p;
        p_cnt[p]++;
    }

    // complex pattern
    Pattern4 p4 = Pattern4::NONE;

    if (p_cnt[Pattern::FIVE] >= 1)
        p4 = Pattern4::A_FIVE;
    else if (color == BLACK && ((p_cnt[Pattern::OVER_LINE] >= 1) ||
                                (p_cnt[Pattern::FOUR] + p_cnt[Pattern::BLOCK_FOUR] >= 2) ||
                                (p_cnt[Pattern::THREE] + p_cnt[Pattern::THREE_S] >= 2)))
        p4 = Pattern4::FORBIDDEN;
    else if (p_cnt[Pattern::BLOCK_FOUR] >= 2 || p_cnt[FOUR] >= 1)
        p4 = Pattern4::FLEX4;
    else if (p_cnt[Pattern::BLOCK_FOUR] >= 1) {
        if (p_cnt[Pattern::THREE] >= 1 || p_cnt[Pattern::THREE_S] >= 1)
            p4 = Pattern4::BLOCK4_FLEX3;
        else if (p_cnt[Pattern::BLOCK_THREE] >= 1)
            p4 = Pattern4::BLOCK4_PLUS;
        else if (p_cnt[Pattern::TWO] + p_cnt[Pattern::TWO_A] + p_cnt[Pattern::TWO_B] >= 1)
            p4 = Pattern4::BLOCK4_PLUS;
        else
            p4 = Pattern4::BLOCK4;
    } else if (p_cnt[Pattern::THREE] >= 1 || p_cnt[Pattern::THREE_S] >= 1) {
        if (p_cnt[Pattern::THREE] + p_cnt[Pattern::THREE_S] >= 2)
            p4 = Pattern4::FLEX3_2X;
        else if (p_cnt[Pattern::BLOCK_THREE] >= 1)
            p4 = Pattern4::FLEX3_PLUS;
        else if (p_cnt[Pattern::TWO] + p_cnt[Pattern::TWO_A] + p_cnt[Pattern::TWO_B] >= 1)
            p4 = Pattern4::FLEX3_PLUS;
        else
            p4 = Pattern4::FLEX3;
    } else if (p_cnt[Pattern::BLOCK_THREE] >= 1)
        p4 = Pattern4::BLOCK3;
    else if (p_cnt[Pattern::TWO] + p_cnt[Pattern::TWO_A] + p_cnt[Pattern::TWO_B] >= 2)
        p4 = Pattern4::FLEX2_2X;
    else if (p_cnt[Pattern::BLOCK_THREE])
        p4 = Pattern4::BLOCK3;
    else if (p_cnt[Pattern::TWO] + p_cnt[Pattern::TWO_A] + p_cnt[Pattern::TWO_B] >= 1)
        p4 = Pattern4::FLEX2;

    score = getPatternScore(p4);

    this->board[x][y] = -1;  // remove temporarily stone

    // adjust score for edge
    // if (x == 0 || y == 0 || x == size - 1 || y == size - 1) {
    //     score *= 0.9;
    // } else if (x == 1 || y == 1 || x == size - 2 || y == size - 2) {
    //     score *= 0.95;
    // }

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
            if (this->pattern_cache[0][d][x][y] > Pattern::DEAD) cnt++;
            if (this->pattern_cache[1][d][x][y] > Pattern::DEAD) cnt++;
        }
    } else {
        for (int d = 0; d < 4; d++) {
            if (this->pattern_cache[color][d][x][y] > Pattern::DEAD) cnt++;
        }
    }

    return cnt;
}

unordered_map<int, unordered_set<int>> Evaluator::getPoints(Color color, int depth, bool vct,
                                                            bool vcf) {
    Color first = depth % 2 ? ~color : color;  // if depth is even, set first as self color
    unordered_map<int, unordered_set<int>> points;

    for (int pattern : {DEAD, OVER_LINE, BLOCK_ONE, ONE, BLOCK_TWO, TWO, TWO_A, TWO_B, BLOCK_THREE,
                        THREE, THREE_S, BLOCK_FOUR, FOUR, Pattern::FIVE}) {
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
                    if (p == Pattern::DEAD) continue;

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
                            if (p != THREE && p != THREE_S && !isFour(p) && !isFive(p)) continue;
                            if ((p == THREE || p == THREE_S) && c != first) continue;
                            if (depth == 0 && c != first) continue;
                            if (depth > 0 && (p == THREE || p == THREE_S || p == BLOCK_FOUR) &&
                                getPatternCountOfPoint(x, y, c) == 1)
                                continue;
                        } else {  // only consider defence
                            if (p != THREE && p != THREE_S && !isFour(p) && !isFive(p)) continue;
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
                        (p == Pattern::TWO || p == Pattern::TWO_A || p == Pattern::TWO_B ||
                         p == Pattern::BLOCK_THREE) &&
                        !isPointInLine(point, last_points)) {
                        continue;
                    }

                    points[p].insert(point);
                }
            }
        }
    }
    return points;
}

vector<int> Evaluator::getMoves(Color color, int depth, bool vct, bool vcf) {
    auto points = getPoints(color, depth, vct, vcf);

    auto fives = points[Pattern::FIVE];
    if (fives.size()) return {fives.begin(), fives.end()};

    unordered_set<int> others;
    vector<int> res;
    for (int i = FOUR; i >= TWO; i--) {
        auto it = points[i].begin();
        while (others.size() < 25 && it != points[i].end()) {
            if (others.find(*it) == others.end()) {
                others.insert(*it);
                res.push_back(*it);
            }
            it++;
        }
        if (others.size() >= 25) return res;
    }
    return res;
}
