#include <cstdlib>
#include <iostream>

#include "core/minimax.h"

#define MAX 100000000

using namespace std;

void aiPlay(Board& board, Color color, int depth) {
    auto [score, move, path] = minimax(board, Color::BLACK, 8, 0, -MAX, MAX, Points{}, true, false);
    if (score >= 100000) {
        board.putStone(move, Color::BLACK);
        cout << "Score: " << score << ", Move: [" << move.first << ", " << move.second << "]\n";
        return;
    }
    tie(score, move, path) = minimax(board, Color::BLACK, 4, 0, -MAX, MAX, Points{}, false, false);
    cout << board.cnt << "\n";
    board.putStone(move, Color::BLACK);
    cout << "Score: " << score << ", Move: [" << move.first << ", " << move.second << "]\n";
}

int main() {
    int size = 15;
    Board board(size);
    aiPlay(board, Color::BLACK, 4);
    board.putStone({7, 8}, Color::WHITE);
    aiPlay(board, Color::BLACK, 4);
}
