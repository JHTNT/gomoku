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

Color Board::getWinner() {
    unsigned long long hash = this->zobrist.get_hash();
    if (this->winner_cache.has(hash)) {
        return this->winner_cache.get(hash);
    }

    for (int x = 0; x < this->size; x++) {
        for (int y = 0; y < this->size; y++) {
            if (this->board[x][y] == Color::EMPTY) continue;
            for (auto d : this->evaluator.all_directions) {
                int cnt = 0;
                int new_x = x + d.first * cnt, new_y = y + d.second * cnt;
                while (new_x >= 0 && new_x < this->size && new_y >= 0 && new_y < this->size &&
                       this->board[new_x][new_y] == this->board[x][y]) {
                    cnt++;
                    new_x = x + d.first * cnt, new_y = y + d.second * cnt;
                }

                if (cnt >= 5) {
                    this->winner_cache.put(hash, Color(this->board[x][y]));
                    return Color(this->board[x][y]);
                }
            }
        }
    }

    this->winner_cache.put(hash, Color::EMPTY);
    return Color::EMPTY;
}

bool Board::isGameOver() {
    unsigned long long hash = this->zobrist.get_hash();
    if (this->gameover_cache.has(hash)) {
        return this->gameover_cache.get(hash);
    }

    if (this->getWinner() != Color::EMPTY) {
        this->gameover_cache.put(hash, true);
        return true;
    }

    // gameover when there is no empty space
    for (int x = 0; x < this->size; x++) {
        for (int y = 0; y < this->size; y++) {
            if (this->board[x][y] == Color::EMPTY) {
                this->gameover_cache.put(hash, false);
                return false;
            }
        }
    }

    this->gameover_cache.put(hash, true);
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

    int score = 0;
    Color winner = this->getWinner();
    if (winner != Color::EMPTY) {
        score = winner == color ? 10000000 : -10000000;
    } else {
        score = this->evaluator.evaluate(color);
    }

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

void Board::printBoard() {
    for (int i = 0; i < this->size; i++) {
        if (i >= 6) cout << " ";
        cout << 15 - i << " ";
        for (int j = 0; j < this->size; j++) {
            if (j != 0) cout << "─";

            if (this->board[i][j] == Color::BLACK) {
                cout << "⬤";
                continue;
            } else if (this->board[i][j] == Color::WHITE) {
                cout << "◯";
                continue;
            }

            if (i == 0 && j == 0) {
                cout << "┌";
                continue;
            }
            if (i == 0 && j == 14) {
                cout << "┐";
                continue;
            }
            if (i == 14 && j == 0) {
                cout << "└";
                continue;
            }
            if (i == 14 && j == 14) {
                cout << "┘";
                continue;
            }
            if (i == 0) {
                cout << "┬";
                continue;
            }
            if (i == 14) {
                cout << "┴";
                continue;
            }
            if (j == 0) {
                cout << "├";
                continue;
            }
            if (j == 14) {
                cout << "┤";
                continue;
            }
            cout << "┼";
        }
        cout << "\n";
    }
    cout << "   A B C D E F G H I J K L M N O\n";
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

    vector<int> moves = this->evaluator.getMoves(color, depth, vct, vcf);
    Points points = Points();
    for (auto move : moves) points.push_back({move / this->size, move % this->size});

    int center = floor(this->size / 2);
    if (this->board[center][center] == -1) {
        points.push_back({center, center});
    }

    this->valuable_moves_cache.put(hash, {color, points, depth, vct, vcf});
    return points;
}

Board Board::reverse() {
    Board reverse_board = Board(this->size, ~this->ai_color);
    for (auto h : this->history) {
        auto [x, y, color] = h;
        reverse_board.putStone({x, y}, ~color);
    }
    return reverse_board;
}
