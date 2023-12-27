#include <cstdlib>
#include <ctime>
#include <iostream>
#include <windows.h>

#include "core/minimax.h"

#define MAX 100000000

using namespace std;

void aiPlay(Board& board, Color color, int depth) {
    clock_t s, e;
    s = clock();
    auto [score, move, path] =
        minimax(board, board.ai_color, depth + 8, 0, -MAX, MAX, Points{}, true, false);
    if (score >= 100000) {
        board.putStone(move, board.ai_color);
        e = clock();
        board.printBoard();
        cout << board.cnt << "\n";
        cout << (double) (e - s) / CLOCKS_PER_SEC << " s\n";
        cout << "Score: " << score << ", Move: [" << move.first << ", " << move.second << "]\n";
        return;
    }
    tie(score, move, path) =
        minimax(board, board.ai_color, depth, 0, -MAX, MAX, Points{}, false, false);
    board.putStone(move, board.ai_color);
    e = clock();
    board.printBoard();
    cout << board.cnt << "\n";
    cout << (double) (e - s) / CLOCKS_PER_SEC << " s\n";
    cout << "Score: " << score << ", Move: [" << move.first << ", " << move.second << "]\n";
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
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

    int row, col, row_input;
    string col_input;
    if (player_color == Color::WHITE) aiPlay(board, board.ai_color, 4);
    while (true) {
        cout << "Enter move[row (1 ~ 15) column (A ~ O)]: ";
        cin >> row_input >> col_input;
        if (row_input == -1 && col_input == "-1") break;
        row = 15 - row_input;
        col = col_input[0] - 'A';
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
