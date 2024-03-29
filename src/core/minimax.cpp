#include "core/minimax.h"

#include <algorithm>
#include <climits>
#include <iostream>

#include "components/cache.h"

using namespace std;

// <remain_depth (depth - current_depth), value, move, color, path, vct, vcf>
Cache<tuple<int, int, Point, Color, Points, bool, bool>> minimax_cache{};

tuple<int, Point, Points> minimax(Board& board, Color color, int depth, int current_depth,
                                  int alpha, int beta, Points path, bool vct, bool vcf) {
    board.cnt++;
    if (current_depth >= depth || board.isGameOver()) {
        return {board.evaluate(color), Point{}, path};
    }

    // check cache
    unsigned long long hash = board.get_hash();
    int remain_depth = INT_MAX;
    if (minimax_cache.has(hash)) {
        auto cache = minimax_cache.get(hash);
        remain_depth = get<0>(cache);
        if (get<3>(cache) == color &&
            (abs(get<1>(cache)) >= 10000000 || remain_depth >= depth - current_depth) &&
            get<5>(cache) == vct && get<6>(cache) == vcf) {
            Points prev_path = get<4>(cache);
            path.insert(path.end(), prev_path.begin(), prev_path.end());
            return {get<1>(cache), get<2>(cache), path};
        }
    }

    int max_value = -100000000;
    Point best_move{};
    Points best_path = Points(path);
    Points moves = board.getValuableMoves(color, current_depth, vct || current_depth > 6, vcf);
    if (moves.size() == 0) {
        return {board.evaluate(color), Point{}, path};
    }
    for (int d = current_depth + 1; d <= depth; d++) {
        if (d % 2) continue;  // only evaluate on self turn
        bool end = false;
        for (auto point : moves) {
            Points new_path(path);
            new_path.push_back(point);
            board.putStone(point, color);
            auto [current_value, current_move, current_path] =
                minimax(board, ~color, d, current_depth + 1, -beta, -alpha, new_path, vct, vcf);
            current_value = -current_value;
            // if (current_depth == 0 && d == depth) {
            //     cout << "[" << point.first << "," << point.second << "] ";
            //     cout << current_value << "\n";
            // }
            board.takeStone();
            if (current_value >= 10000000 || d == depth) {
                if (current_value > max_value ||
                    (current_value <= -10000000 && max_value <= -10000000 &&
                     current_path.size() > best_path.size())) {
                    max_value = current_value;
                    best_move = point;
                    best_path = path;
                }
            }
            alpha = max(max_value, alpha);
            if (alpha >= 10000000) {  // self win
                end = true;
                break;
            }
            if (alpha >= beta) break;
        }
        if (end) break;
    }

    // update cache
    if ((current_depth <= 6 || vct || vcf) &&
        (!minimax_cache.has(hash) || remain_depth < depth - current_depth)) {
        if (best_path.size() >= current_depth) {
            minimax_cache.put(
                hash, {depth - current_depth, max_value, best_move, color,
                       Points(best_path.begin(), best_path.begin() + current_depth), vct, vcf});
        } else {
            minimax_cache.put(
                hash, {depth - current_depth, max_value, best_move, color, best_path, vct, vcf});
        }
    }
    return {max_value, best_move, best_path};
}
