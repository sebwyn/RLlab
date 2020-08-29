#include "Tile.hpp"

Tile TileManager::wall = {glm::vec4(1, 0, 0, 1)};
Tile TileManager::floor = {glm::vec4(0, 0, 0, 1)};
Tile TileManager::special = {glm::vec4(1, 1, 0, 1)};
Tile TileManager::player = {glm::vec4(1, 1, 1, 1)};
