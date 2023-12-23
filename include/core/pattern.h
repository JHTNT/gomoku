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
inline bool isFour(Pattern pattern) { return (pattern == FOUR || pattern == BLOCK_FOUR); }
inline bool isFive(Pattern pattern) { return (pattern == FIVE || pattern == BLOCK_FIVE); }
#endif
