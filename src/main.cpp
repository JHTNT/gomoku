#include <cstdlib>
#include <iostream>

#include "core/minimax.h"

#define MAX 100000000

using namespace std;

void aiPlay(Board& board, Color color, int depth) {
    auto [score, move, path] =
        minimax(board, board.ai_color, 12, 0, -MAX, MAX, Points{}, true, false);
    if (score >= 100000) {
        board.putStone(move, board.ai_color);
        cout << "Score: " << score << ", Move: [" << move.first << ", " << move.second << "]\n";
        return;
    }
    tie(score, move, path) =
        minimax(board, board.ai_color, depth, 0, -MAX, MAX, Points{}, false, false);
    cout << board.cnt << "\n";
    board.putStone(move, board.ai_color);
    cout << "Score: " << score << ", Move: [" << move.first << ", " << move.second << "]\n";
}

int main() {
    int size = 15;
    char color_input;
    Color player_color;
    while (true) {
        cout << "Choose White or Black[b/w]: ";
        cin >> color_input;
        if (color_input == 'w' || color_input == 'W') {
            player_color = Color::WHITE;
            break;
        } else if (color_input == 'b' || color_input == 'B') {
            player_color = Color::BLACK;
            break;
        }
    }
    Board board(size, ~player_color);

    int row, col;
    if (player_color == Color::WHITE) aiPlay(board, board.ai_color, 4);
    while (true) {
        cout << "Enter move[row column (0 ~ 14)]: ";
        cin >> row >> col;
        if (row == -1 && col == -1) break;
        if (board.checkMove(row, col)) {
            board.putStone({row, col}, player_color);
        } else {
            cout << "Invalid move\n";
            continue;
        }
        board.cnt = 0;
        aiPlay(board, board.ai_color, 4);
    }
}
