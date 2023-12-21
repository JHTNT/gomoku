#ifndef TYPES
#define TYPES
#include <vector>

typedef std::pair<int, int> Point;
typedef std::vector<std::pair<int, int>> Points;

enum Color { BLACK, WHITE, EMPTY = -1 };

constexpr Color operator~(Color c) { return Color(c ^ 1); }
#endif

enum Pattern {
    FIVE = 5,
    BLOCK_FIVE = 50,
    FOUR = 4,
    FOUR_FOUR = 44,    // 雙沖四
    FOUR_THREE = 43,   // 沖四活三
    THREE_THREE = 33,  // 雙三
    BLOCK_FOUR = 40,
    THREE = 3,
    BLOCK_THREE = 30,
    TWO_TWO = 22,  // 雙活二
    TWO = 2,
    NONE = 0,
};
