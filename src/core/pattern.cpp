#include "core/pattern.h"

using namespace std;

tuple<int, int, int, int, int, int> countPattern(const vector<vector<short>>& board, int x, int y,
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
        }
        total_length++;
        if (current_color == Color::EMPTY) {
            temp_empty_cnt++;
            side_empty_cnt++;
        }
        if (side_empty_cnt >= 2) break;
    }

    if (!inner_empty_cnt) one_empty_self_cnt = 0;
    return {inner_empty_cnt, total_length,      side_empty_cnt,
            self_cnt,        no_empty_self_cnt, one_empty_self_cnt};
}

Pattern getPattern(const std::vector<std::vector<short>>& board, int x, int y, int offset_x,
                   int offset_y, Color color) {
    // pattern source: https://587.renju.org.tw/teach/teach029.htm

    // skip empty point
    if (x > 1 && x < board.size() - 2 && y > 1 && y < board.size() - 2 &&
        board[x + offset_x][y + offset_y] == -1 && board[x - offset_x][y - offset_y] == -1 &&
        board[x + 2 * offset_x][y + 2 * offset_y] == -1 &&
        board[x - 2 * offset_x][y - 2 * offset_y] == -1) {
        return Pattern::ONE;
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

    if (total_length < 5) return Pattern::DEAD;  // can't form any pattern

    // forbidden pattern
    if (no_empty_self_cnt > 5 && color == Color::BLACK) return Pattern::OVER_LINE;

    // five
    if (no_empty_self_cnt >= 5) {
        return Pattern::FIVE;
    }

    // four
    if (no_empty_self_cnt == 4) {
        if (pos_empty >= 1 && neg_empty >= 1)
            return Pattern::FOUR;  // .OOOO.
        else
            return Pattern::BLOCK_FOUR;  // XOOOO.
    } else if (one_empty_self_cnt == 4) {
        return Pattern::BLOCK_FOUR;  // OOO.O or OO.OO
    }

    // three
    if (no_empty_self_cnt == 3) {
        if ((pos_empty >= 1 && neg_empty >= 2) || (pos_empty >= 2 && neg_empty >= 1)) {
            return Pattern::THREE_S;  // _.OOO_ or _OOO._
        } else {
            return Pattern::BLOCK_THREE;  // XOOO.._ or X.OOO.X
        }
    } else if (one_empty_self_cnt == 3) {
        if (pos_empty >= 1 && neg_empty >= 1) {
            return Pattern::THREE;  // _OO.O_
        } else {
            return Pattern::BLOCK_THREE;  // XOO.O._ or XO.OO._
        }
    } else if (pos_one_empty_self == 1 && neg_one_empty_self == 1) {
        return Pattern::BLOCK_THREE;  // _O.O.O_
    } else if (self_cnt == 3 && pos_inner_empty + neg_inner_empty == 2) {
        return Pattern::BLOCK_THREE;  // _O.O.O_ or _OO..O_
    }

    // two
    if (no_empty_self_cnt == 2) {
        if (pos_empty >= 2 && neg_empty >= 2)
            return Pattern::TWO_B;  // _..OO.._
        else
            return Pattern::BLOCK_TWO;  // XOO..._ or X.OO..X
    } else if (one_empty_self_cnt == 2) {
        if (pos_empty >= 1 && neg_empty >= 1)
            return Pattern::TWO_A;  // _.O.O._
        else
            return Pattern::BLOCK_TWO;  // XO.O.._ or X.O.O.X
    } else if (self_cnt == 2 && pos_inner_empty + neg_inner_empty == 2) {
        if (pos_empty >= 1 && neg_empty >= 1)
            return Pattern::TWO;  // _O..O_
        else
            return Pattern::BLOCK_TWO;  // XO..O._
    }

    // one
    if (no_empty_self_cnt == 1) {
        if (pos_empty >= 3 && neg_empty >= 3) {
            return Pattern::ONE;
        } else {
            return Pattern::BLOCK_ONE;
        }
    }

    return Pattern::DEAD;
}

// get the score before putting this stone
int getPatternScore(Pattern4 pattern) {
    switch (pattern) {
        case Pattern4::A_FIVE:
            return 5000;
        case Pattern4::FLEX4:
            return 1000;
        case Pattern4::BLOCK4_FLEX3:
            return 300;
        case Pattern4::BLOCK4_PLUS:
            return 200;
        case Pattern4::BLOCK4:
            return 160;
        case Pattern4::FLEX3_2X:
            return 90;
        case Pattern4::FLEX3_PLUS:
            return 70;
        case Pattern4::FLEX3:
            return 65;
        case Pattern4::BLOCK3_PLUS:
            return 60;
        case Pattern4::FLEX2_2X:
            return 30;
        case Pattern4::BLOCK3:
            return 20;
        case Pattern4::FLEX2:
            return 10;
        case Pattern4::FORBIDDEN:
            return -10000;
        default:
            return 0;
    }
}
