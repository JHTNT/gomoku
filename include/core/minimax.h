#ifndef MINIMAX
#define MINIMAX
#include <tuple>
#include <vector>

#include "components/board.h"
#include "components/types.h"

std::tuple<int, Point, Points> minimax(Board board, Color color, int depth, int current_depth,
                                       int alpha, int beta, Points path);
#endif
