#ifndef BOARD
#define BOARD
#include <vector>

#include "components/cache.h"
#include "components/zobrist.h"
#include "core/eval.h"

class Board {
    const int size;
    Color next_color;
    Evaluator evaluator;
    std::vector<std::vector<short>> board;
    std::vector<std::tuple<int, int, Color>> history;

    Zobrist zobrist;
    // <color, score>
    Cache<std::pair<Color, int>> evaluation_cache;
    // <color, moves, depth, vct, vcf>
    Cache<std::tuple<Color, Points, int, bool, bool>> valuable_moves_cache;

   public:
    const Color ai_color;

    int cnt = 0;
    Board(int n, Color color);
    bool isGameOver();
    int evaluate(Color color);
    bool checkMove(int x, int y);
    void putStone(Point move, Color color);
    void takeStone();
    void printBoard();
    Points getValuableMoves(Color color, int depth, bool vct, bool vcf);

    int get_pixel(int i, int j) { return this->board[i][j]; }
    int get_size() { return this->size; }
    unsigned long long get_hash() { return this->zobrist.get_hash(); }
};
#endif
