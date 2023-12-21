#ifndef PATTERN
#define PATTERN
#include <tuple>

#include "components/types.h"

std::tuple<int, int, int, int, int, int> countPattern(std::vector<std::vector<short>> board, int x,
                                                      int y, int offset_x, int offset_y,
                                                      Color color);
Pattern getPattern(std::vector<std::vector<short>> board, int x, int y, int offset_x, int offset_y,
                   Color color);
int getPatternScore(Pattern pattern);
#endif
