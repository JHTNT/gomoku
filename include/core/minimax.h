#ifndef MINIMAX
#define MINIMAX
#include <tuple>
#include <vector>

#include "components/Board.h"
#include "components/Types.h"

std::tuple<int, Point, Points> minimax(Board board, Color color, int depth, int alpha, int beta,
                                       Points path);
#endif
