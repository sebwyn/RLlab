#include "Tile.hpp"

Tile TileManager::wall = {' ', 1};
Tile TileManager::floor = {' ', 2};
Tile TileManager::special = {'#', 2};
Tile TileManager::player = {'@', 2};