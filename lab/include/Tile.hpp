#pragma once

#include <vector>
#include <curses.h>

#include <glm/glm.hpp>

struct Tile {
    glm::vec4 color;

    bool operator==(Tile& other) const {
        if(color == other.color) return true;
        else return false;
    }
};

class TileManager {
public:
    static Tile wall;
    static Tile floor;
    static Tile special;
    static Tile player;
};
