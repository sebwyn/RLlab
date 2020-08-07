#pragma once

#include <vector>
#include <string>

#include "Tile.hpp"

class LevelGenerator {
public:
    LevelGenerator(int width, int height) : m_width(width), m_height(height) {}

    virtual void generate(std::vector<std::vector<Tile>>& tiles);
protected:
    int m_width, m_height;
    std::vector<std::vector<Tile>>* m_tiles; 
    
    void populateTiles(char sym);
};
