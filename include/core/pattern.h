#ifndef PATTERN
#define PATTERN
#include <tuple>

#include "components/types.h"

std::tuple<int, int, int, int, int, int> countPattern(const std::vector<std::vector<short>>& board,
                                                      int x, int y, int offset_x, int offset_y,
                                                      Color color);
Pattern getPattern(const std::vector<std::vector<short>>& board, int x, int y, int offset_x,
                   int offset_y, Color color);
int getPatternScore(Pattern4 pattern);
inline bool isFour(Pattern pattern) { return (pattern == FOUR || pattern == BLOCK_FOUR); }
inline bool isFive(Pattern pattern) { return pattern == FIVE; }
#endif
