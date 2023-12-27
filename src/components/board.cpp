#include "components/board.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace std;

Board::Board(int n, Color color)
    : size{n},
      ai_color{color},
      evaluator{Evaluator(n)},
      board{vector<vector<short>>(n, vector<short>(n, -1))},
      zobrist{Zobrist(n)} {}

bool Board::isGameOver() {
    for (int x = 0; x < this->size; x++) {
        for (int y = 0; y < this->size; y++) {
            if (this->board[x][y] == -1) continue;
            for (auto d : this->evaluator.all_directions) {
                int cnt = 0;
                int new_x = x + d.first * cnt, new_y = y + d.second * cnt;
                while (new_x < 0 && new_x >= this->size && new_y < 0 && new_y >= this->size &&
                       this->board[new_x][new_y] == this->board[x][y]) {
                    cnt++;
                }
                if (cnt >= 5) return true;
            }
        }
    }

    for (int x = 0; x < this->size; x++) {
        for (int y = 0; y < this->size; y++) {
            if (this->board[x][y] == -1) return false;
        }
    }
    return true;
}

int Board::evaluate(Color color) {
    unsigned long long hash = this->zobrist.get_hash();
    if (this->evaluation_cache.has(hash)) {
        auto cache = this->evaluation_cache.get(hash);
        if (get<0>(cache) == color) {
            return get<1>(cache);
        }
    }
    int score = this->evaluator.evaluate(color);
    this->evaluation_cache.put(hash, {color, score});
    return score;
}

bool Board::checkMove(int x, int y) {
    if (x < 0 || x >= this->size || y < 0 || y >= this->size)
        return false;
    else if (this->board[x][y] != -1)
        return false;
    else
        return true;
}

void Board::putStone(Point move, Color color) {
    int x = move.first, y = move.second;
    this->board[x][y] = color;
    this->history.push_back({x, y, color});
    this->zobrist.modifyBoard(x, y, color);
    this->evaluator.putStone(x, y, color);
}

void Board::takeStone() {
    auto [last_x, last_y, last_color] = this->history.back();
    this->history.pop_back();
    this->board[last_x][last_y] = -1;
    this->zobrist.modifyBoard(last_x, last_y, last_color);
    this->evaluator.takeStone(last_x, last_y, last_color);
}

Points Board::getValuableMoves(Color color, int depth, bool vct, bool vcf) {
    unsigned long long hash = this->zobrist.get_hash();
    if (this->valuable_moves_cache.has(hash)) {
        auto cache = this->valuable_moves_cache.get(hash);
        if (get<0>(cache) == color && get<2>(cache) == depth && get<3>(cache) == vct &&
            get<4>(cache) == vcf) {
            return get<1>(cache);
        }
    }
    unordered_set<int> moves = this->evaluator.getMoves(color, depth, vct, vcf);
    Points points = Points();
    for (auto move : moves) points.push_back({move / this->size, move % this->size});

    int center = floor(this->size / 2);
    if (this->board[center][center] == -1) {
        points.push_back({center, center});
    }

    this->valuable_moves_cache.put(hash, {color, points, depth, vct, vcf});
    return points;
}
