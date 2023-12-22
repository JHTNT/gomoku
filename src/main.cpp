#include <cstdlib>
#include <iostream>

#include "core/minimax.h"

using namespace std;

int main() {
    int size = 15;
    Board board(size);
    auto [score, move, path] = minimax(board, Color::BLACK, 4, 0, INT_MIN, INT_MAX, Points{});
    cout << "Score: " << score << ", Move: [" << move.first << ", " << move.second << "]\n";
}
