#include "core/minimax.h"

#include <algorithm>
#include <climits>
#include <iostream>

using namespace std;

tuple<int, Point, Points> minimax(Board& board, Color color, int depth, int current_depth, int alpha,
                                  int beta, Points path, bool vct, bool vcf) {
    board.cnt++;
    if (current_depth >= depth || board.isGameOver()) {
        return {board.evaluate(color), Point{}, path};
    }

    int max_value = -100000000;
    Point move{};
    Point best_move{};
    Points best_path = Points(path);
    Points moves = board.getValuableMoves(color, depth, vct, vcf);
    if (moves.size() == 0) {
        return {board.evaluate(color), Point{}, path};
    }
    for (int d = current_depth + 1; d <= depth; d++) {
        if (d % 2) continue;  // only evaluate on self turn
        bool end = false;
        for (auto point : moves) {
            vector<pair<int, int>> new_path = vector<pair<int, int>>(path);
            new_path.push_back(point);
            board.putStone(point, color);
            auto [current_value, current_move, current_path] =
                minimax(board, ~color, d, current_depth + 1, -beta, -alpha, new_path, vct, vcf);
            current_value = -current_value;
            board.takeStone(point, color);
            if (current_value > 100000 || d == depth) {
                if (current_value > max_value) {
                    max_value = current_value;
                    best_move = point;
                    best_path = path;
                }
            }
            alpha = max(max_value, alpha);
            if (alpha >= 100000) {  // self win
                end = true;
                break;
            }
            if (alpha >= beta) break;
        }
        if (end) break;
    }
    return {max_value, best_move, best_path};
}
