#include "BasicDungeonGenerator.hpp"

#include <tuple>

void BasicDungeonGenerator::update(){
}

void BasicDungeonGenerator::handleInput(int key){
    switch(key){
    }
}

void BasicDungeonGenerator::generate(){
    initCells();
    for(int i = 0; i < m_attempts; i++){
        Room room = {m_colDistribution(m_generator), m_rowDistribution(m_generator), m_sizeDistribution(m_generator), m_sizeDistribution(m_generator)}; 
        if(validateRoom(room)){
            placeRoom(room);
            m_rooms.push_back(room);
        }
    }
    stepMaze(0, 0);
}


void BasicDungeonGenerator::initCells(){
    for(int r = 0; r < m_wHeight; r++){
        m_game->getWorld().push_back(std::vector<Tile>());
        if(r % 2 == 1) m_visited.push_back(std::vector<bool>());
        for(int c = 0; c < m_wWidth; c++){
            if(r % 2 == 1 && c % 2 == 1){
                m_game->getWorld().back().push_back(TileManager::floor);
                m_visited.back().push_back(false);
            } else { 
                m_game->getWorld().back().push_back(TileManager::wall);
            }
        }
    }
}

bool BasicDungeonGenerator::validateRoom(Room room){
    if(room.x + room.width < m_width && room.y + room.height < m_height){
        for(Room r : m_rooms){
            //check to make sure the rooms don't collide
            if((r.x <= room.x && room.x < r.x + r.width) || (room.x <= r.x && r.x < room.x + room.width)){
                if((r.y <= room.y && room.y < r.y + r.height) || (room.y <= r.y && r.y < room.y + room.height)){
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}

void BasicDungeonGenerator::placeRoom(Room room){
    //need to convert rooms in cell space to rooms in world space
    for(int r = room.y*2; r < (room.y + room.height)*2+1; r++){
        for(int c = room.x*2; c < (room.x + room.width)*2+1; c++){
            if(c == room.x*2 || c == (room.x + room.width)*2 || r == room.y*2 || r == (room.y + room.height)*2){
                //this tile is a wall on the border of a room
                m_game->getWorld()[r][c] = TileManager::wall;
            } else {
                if(r % 2 == 1 && c % 2 == 1) m_visited[r/2][c/2] = true;
                m_game->getWorld()[r][c] = TileManager::floor;
            }
        }
    }
}

void BasicDungeonGenerator::generateMaze(){
    //generate all the cells and walls 
    stepMaze(0, 0);
    //mazeGenerator = new std::thread(&BasicDungeonGenerator::stepMaze, this, 0, 0);
}

void BasicDungeonGenerator::stepMaze(int cr, int cc){
    static int* stackStart = &cr;
    std::cout << "The stack is " << stackStart - &cr << " bytes" << std::endl;
    struct DirectionCell {
        char dir;
        int r, c;
    };

    m_visited[cr][cc] = true;
    std::vector<DirectionCell> neighbors;
    neighbors.reserve(4);
    //populate neighbors
    if(cr > 0            && !m_visited[cr-1][cc]) neighbors.push_back({'N', cr-1, cc});
    if(cc > 0            && !m_visited[cr][cc-1]) neighbors.push_back({'W', cr, cc-1});
    if(cc+1 < m_width    && !m_visited[cr][cc+1]) neighbors.push_back({'E', cr, cc+1});
    if(cr+1 < m_height   && !m_visited[cr+1][cc]) neighbors.push_back({'S', cr+1, cc});  
    while(neighbors.size() > 0){
        //pick a neighbor 
        std::uniform_int_distribution<int> directonPicker(0, neighbors.size()-1);
        int selectedNum = directonPicker(m_generator);
        DirectionCell selected = neighbors[selectedNum]; 
        //cells visited status may have changed
        if(!m_visited[selected.r][selected.c]){
            //open the wall leading to the neighbor
            int worldRow = cr*2+1; int worldCol = cc*2+1;
            switch(selected.dir){
                case 'N': m_game->getWorld()[worldRow-1][worldCol] = TileManager::floor; break;
                case 'W': m_game->getWorld()[worldRow][worldCol-1] = TileManager::floor; break;
                case 'E': m_game->getWorld()[worldRow][worldCol+1] = TileManager::floor; break;
                case 'S': m_game->getWorld()[worldRow+1][worldCol] = TileManager::floor; break;
            }
            stepMaze(selected.r, selected.c);
        }
        neighbors.erase(neighbors.begin()+selectedNum);
    }
}
