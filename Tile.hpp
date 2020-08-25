#pragma once

#include <vector>
#include <curses.h>

struct Tile {
    char sym;
    int color;
};

class TileManager {
public:
    static Tile wall;
    static Tile floor;
    static Tile special;

    static void initColors(){
        start_color();

        init_pair(1, COLOR_BLACK, COLOR_RED);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
    };
};
