#ifndef TYPES
#define TYPES
typedef std::pair<int, int> Point;
typedef std::vector<std::pair<int, int>> Points;

enum Color { Black, White };

constexpr Color operator~(Color c) { return Color(c ^ 1); }
#endif
