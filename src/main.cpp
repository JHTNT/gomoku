#include <windows.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

#include "core/minimax.h"

#define MAX 100000000

using namespace std;

void aiPlay(Board& board, Color color, int depth) {
    auto [score, move, path] =
        minimax(board, board.ai_color, depth + 8, 0, -MAX, MAX, Points{}, true, false);

    if (score >= 10000000) {
        board.putStone(move, board.ai_color);
        board.printBoard();
        cout << "Score: " << score << ", Move: [" << (15 - move.first) << ", "
             << (char) (move.second + 'A') << "]\n";
        return;
    }

    tie(score, move, path) =
        minimax(board, board.ai_color, depth, 0, -MAX, MAX, Points{}, false, false);
    board.putStone(move, board.ai_color);
    Board board2 = board.reverse();
    auto [score2, move2, path2] =
        minimax(board2, board.ai_color, depth + 8, 0, -MAX, MAX, Points{}, true, false);
    board.takeStone();

    if (score < 10000000 && score2 == 10000000 && path2.size() > path.size()) {
        Board board3 = board.reverse();
        auto [score3, move3, path3] =
            minimax(board3, board.ai_color, depth + 8, 0, -MAX, MAX, Points{}, true, false);
        if (path2.size() <= path3.size()) {
            board.putStone(move2, board.ai_color);
            board.printBoard();
            cout << "Score: " << score << ", Move: [" << (15 - move.first) << ", "
                 << (char) (move.second + 'A') << "]\n";
            return;
        }
    }

    board.putStone(move, board.ai_color);
    board.printBoard();
    cout << "Score: " << score << ", Move: [" << (15 - move.first) << ", "
         << (char) (move.second + 'A') << "]\n";
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    int size = 15;
    int depth;
    char color_input;
    Color player_color;
    clock_t s, e;

    cout << "Enter search depth: ";
    cin >> depth;

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
    bool ai_first_stone = false;
    if (player_color == Color::WHITE) {
        aiPlay(board, board.ai_color, 4);
    } else {
        ai_first_stone = true;
        board.printBoard();
    }

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
        s = clock();
        if (ai_first_stone) {
            aiPlay(board, board.ai_color, 4);
            ai_first_stone = false;
        } else {
            aiPlay(board, board.ai_color, depth);
        }
        e = clock();
        cout << board.cnt << "\n";
        cout << (double) (e - s) / CLOCKS_PER_SEC << " s\n";
    }
}
