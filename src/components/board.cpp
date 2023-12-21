#include "components/board.h"

#include <cstdlib>
#include <iostream>

using namespace std;

Board::Board(int n)
    : size{n},
      next_color{BLACK},
      evaluator{Evaluator(n, *this)},
      board{vector<vector<short>>(n, vector<short>(n, -1))} {}

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
