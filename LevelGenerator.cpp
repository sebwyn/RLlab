#include "LevelGenerator.hpp"

void LevelGenerator::generate(std::vector<std::vector<Tile>>& tiles){
    m_tiles = &tiles;
    populateTiles('.');
}

void LevelGenerator::populateTiles(char sym){
    for(int r = 0; r < m_height; r++){
        m_tiles->push_back(std::vector<Tile>());
        for(int c = 0; c < m_width; c++){
            m_tiles->back().push_back(Tile(sym)); 
        }
    }
}
