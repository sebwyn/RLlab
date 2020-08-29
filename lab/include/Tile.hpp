#pragma once

#include <vector>
#include <curses.h>

struct Tile {
    char sym;
    int color;

    bool operator==(Tile& other) const {
        if(sym == other.sym && color == other.color) return true;
        else return false;
    }
};

class TileManager {
public:
    static Tile wall;
    static Tile floor;
    static Tile special;
    static Tile player;

    static void initColors(){
        start_color();

        init_pair(1, COLOR_BLACK, COLOR_RED);
        init_pair(2, COLOR_WHITE, COLOR_BLACK);
    };
};
