#include "components/board.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace std;

Board::Board(int n)
    : size{n},
      next_color{BLACK},
      evaluator{Evaluator(n)},
      board{vector<vector<short>>(n, vector<short>(n, -1))} {}

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

int Board::evaluate(Color color) { return this->evaluator.evaluate(color); }

void Board::putStone(Point move, Color color) {
    int x = move.first, y = move.second;
    if (this->board[x][y] != -1) cout << "Invalid move " << x << ", " << y;

    this->board[x][y] = color;
    this->next_color = ~this->next_color;
    this->evaluator.putStone(x, y, color);
}

void Board::takeStone(Point move, Color color) {
    int x = move.first, y = move.second;
    this->board[x][y] = -1;
    this->next_color = ~this->next_color;
    this->evaluator.takeStone(x, y, color);
}

Points Board::getValuableMoves(Color color, int depth) {
    unordered_set<int> moves = this->evaluator.getMoves(color, depth);
    Points points = Points();
    for (auto move : moves) points.push_back({move / this->size, move % this->size});

    int center = floor(this->size / 2);
    if (this->board[center][center] == -1) {
        points.push_back({center, center});
    }
    return points;
}
