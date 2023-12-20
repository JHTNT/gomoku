#include <vector>

#include "components/Types.h"

class Board {
   public:
    Color next_color;

    bool isGameOver();
    int evaluate(Color color);
    std::vector<std::pair<int, int>> getValuableMoves();
    Board putStone(Point move, Color color);
    Board takeStone(Point move, Color color);
};
