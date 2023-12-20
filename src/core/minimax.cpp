#include "core/minimax.h"

#include <algorithm>
#include <climits>

using namespace std;

tuple<int, Point, Points> minimax(Board board, Color color, int depth, int alpha, int beta,
                                  Points path) {
    if (depth == 0 || board.isGameOver()) {
        return {board.evaluate(color), Point{}, path};
    }

    int max_value = INT_MIN;
    Point move;
    Point best_move{};
    Points moves = board.getValuableMoves();
    Points best_path = Points(path);
    for (auto point : moves) {
        vector<pair<int, int>> new_path = vector<pair<int, int>>(path);
        new_path.push_back(point);
        board.putStone(point, color);
        auto [current_value, current_move, current_path] =
            minimax(board, ~color, depth - 1, -beta, -alpha, new_path);
        board.takeStone(point, color);
        if (current_value > max_value) {
            max_value = current_value;
            best_move = point;
            best_path = path;
        }
        alpha = max(current_value, alpha);
        if (alpha >= beta) break;
    }
    return {max_value, best_move, best_path};
}
