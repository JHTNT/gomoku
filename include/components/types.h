#ifndef TYPES
#define TYPES
#include <vector>

typedef std::pair<int, int> Point;
typedef std::vector<std::pair<int, int>> Points;

enum Color { BLACK, WHITE, EMPTY = -1 };

constexpr Color operator~(Color c) { return Color(c ^ 1); }

enum Pattern {
    DEAD,
    OVER_LINE,
    BLOCK_ONE,
    ONE,
    BLOCK_TWO,
    TWO,    // _O__._
    TWO_A,  // _O_.__
    TWO_B,  // _O.___
    BLOCK_THREE,
    THREE,    // _OO_._
    THREE_S,  // __OO.__
    BLOCK_FOUR,
    FOUR,
    FIVE
};

// pattern combined by 4 directions
enum Pattern4 {
    NONE,
    FORBIDDEN,
    FLEX2,
    BLOCK3,
    FLEX2_2X,
    BLOCK3_PLUS,
    FLEX3,
    FLEX3_PLUS,
    FLEX3_2X,
    BLOCK4,
    BLOCK4_PLUS,
    BLOCK4_FLEX3,
    FLEX4,
    A_FIVE
};
#endif
