#include <cstdlib>
#include <iostream>

#include "core/minimax.h"

using namespace std;

int main() {
    int size = 15;
    Board board(size);
    auto [score, move, path] = minimax(board, Color::BLACK, 4, 0, -100000000, 100000000, Points{});
    board.putStone(move, Color::BLACK);
    cout << "Score: " << score << ", Move: [" << move.first << ", " << move.second << "]\n";
    board.putStone({7, 8}, Color::WHITE);
    tie(score, move, path) = minimax(board, Color::BLACK, 4, 0, -100000000, 100000000, Points{});
    board.putStone(move, Color::BLACK); 
    cout << "Score: " << score << ", Move: [" << move.first << ", " << move.second << "]\n";
    
}
