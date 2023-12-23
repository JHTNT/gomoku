#include "core/pattern.h"

using namespace std;

tuple<int, int, int, int, int, int> countPattern(vector<vector<short>> board, int x, int y,
                                                 int offset_x, int offset_y, Color color) {
    int size = board.size();
    Color opposite = ~color;

    int inner_empty_cnt = 0;  // empty slot in the pattern
    int temp_empty_cnt = 0;
    int side_empty_cnt = 0;
    int self_cnt = 0;
    int total_length = 0;
    int no_empty_self_cnt = 0, one_empty_self_cnt = 0;

    for (int i = 1; i <= 5; i++) {
        int new_x = x + i * offset_x, new_y = y + i * offset_y;
        if (new_x >= size || new_y >= size || new_x < 0 || new_y < 0) {
            break;  // can't continue conting pattern of self color
        }

        Color current_color = Color(board[new_x][new_y]);
        if (current_color == opposite) break;

        if (current_color == color) {
            self_cnt++;
            side_empty_cnt = 0;
            if (temp_empty_cnt) {
                inner_empty_cnt += temp_empty_cnt;
                temp_empty_cnt = 0;
            }
            if (inner_empty_cnt == 0) {
                no_empty_self_cnt++;
                one_empty_self_cnt++;
            } else if (inner_empty_cnt == 1) {
                one_empty_self_cnt++;
            }
        } else if (current_color == Color::EMPTY) {
            temp_empty_cnt++;
            side_empty_cnt++;
        }
        total_length++;

        if (side_empty_cnt >= 2) break;
    }

    if (!inner_empty_cnt) one_empty_self_cnt = 0;
    return {inner_empty_cnt, total_length,      side_empty_cnt,
            self_cnt,        no_empty_self_cnt, one_empty_self_cnt};
}

Pattern getPattern(std::vector<std::vector<short>> board, int x, int y, int offset_x, int offset_y,
                   Color color) {
    // skip empty point
    if (x > 1 && x < board.size() - 2 && y > 1 && y < board.size() - 2 &&
        board[x + offset_x][y + offset_y] == -1 && board[x - offset_x][y - offset_y] == -1 &&
        board[x + 2 * offset_x][y + 2 * offset_y] == -1 &&
        board[x - 2 * offset_x][y - 2 * offset_y] == -1) {
        return Pattern::NONE;
    }

    auto [pos_inner_empty, pos_total_len, pos_side_empty, pos_self, pos_no_empty_self,
          pos_one_empty_self] = countPattern(board, x, y, offset_x, offset_y, color);
    auto [neg_inner_empty, neg_total_len, neg_side_empty, neg_self, neg_no_empty_self,
          neg_one_empty_self] = countPattern(board, x, y, -offset_x, -offset_y, color);

    int self_cnt = pos_self + neg_self + 1;
    int total_length = pos_total_len + neg_total_len + 1;
    int no_empty_self_cnt = pos_no_empty_self + neg_no_empty_self + 1;
    int one_empty_self_cnt =
        max(pos_one_empty_self + neg_no_empty_self, pos_no_empty_self + neg_one_empty_self) + 1;
    int pos_empty = pos_side_empty, neg_empty = neg_side_empty;

    if (total_length < 5) return Pattern::NONE;  // can't form any pattern

    // five
    if (no_empty_self_cnt >= 5) {
        if (pos_empty > 0 && neg_empty > 0) {
            return Pattern::FIVE;
        } else {
            return Pattern::BLOCK_FIVE;
        }
    }

    // four
    if (no_empty_self_cnt == 4) {
        if ((pos_empty >= 1 || pos_one_empty_self > pos_no_empty_self) &&
            (neg_empty >= 1 || neg_one_empty_self > neg_no_empty_self)) {
            return Pattern::FOUR;
        } else if (pos_empty != 0 || neg_empty != 0) {
            return Pattern::BLOCK_FOUR;
        }
    }
    if (one_empty_self_cnt == 4) {
        return Pattern::BLOCK_FOUR;
    }

    // three
    if (no_empty_self_cnt == 3) {
        if ((pos_empty >= 1 && neg_empty >= 2) || (pos_empty >= 2 && neg_empty >= 1)) {
            return Pattern::THREE;
        } else if (pos_empty != 0 || neg_empty != 0) {
            return Pattern::BLOCK_THREE;
        }
    } else if (one_empty_self_cnt == 3) {
        if (pos_empty >= 1 && neg_empty >= 1) {
            return Pattern::THREE;
        } else {
            return Pattern::BLOCK_THREE;
        }
    }

    // two
    if ((no_empty_self_cnt == 2 || one_empty_self_cnt == 2) && total_length > 5) {
        return Pattern::TWO;
    }

    return Pattern::NONE;
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
