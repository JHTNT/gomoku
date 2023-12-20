#include "components/board.h"

#include <cstdlib>
#include <iostream>

using namespace std;

Board::Board(int n)
    : size{n},
      next_color{Black},
      evaluator{Evaluator(n, *this)},
      board{vector<vector<short>>(n, vector<short>(n, -1))} {}

void Board::putStone(Point move, Color color) {
    int i = move.first, j = move.second;
    if (this->board[i][j] != -1) cout << "Invalid move " << i << ", " << j;

    this->board[i][j] = color;
    this->next_color = ~this->next_color;
    this->evaluator.updatePoint(i, j, color);
}
