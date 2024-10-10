#include <raylib.h>
#include <stdlib.h>
#ifndef TYPES
#define TYPES

typedef struct Cell {
    bool textured;
    bool bound;
    Color sides[4];
    Color** TextureColors;
    Image* img;
} Cell;

typedef struct map {
    size_t rows;
    size_t cols;
    Cell **cells;
} map;

enum SIDE {
    NORTH = 0,
    SOUTH,
    EAST,
    WEST
};

#endif // !TYPES
