#include "BasicDungeonGenerator.hpp"

#include <tuple>

void BasicDungeonGenerator::update(){
}

void BasicDungeonGenerator::handleInput(int key){
    switch(key){
        case ' ':
            {
                std::unique_lock<std::mutex> lk(m_mtx);
                m_shouldStep = true;
                m_cv.notify_one();
            }
            break;
    }
}

void BasicDungeonGenerator::generate(){
    initCells();
    for(int i = 0; i < m_attempts; i++){
        Room room = {m_colDistribution(m_generator), m_rowDistribution(m_generator), m_sizeDistribution(m_generator), m_sizeDistribution(m_generator), false}; 
        if(validateRoom(room)){
            placeRoom(room);
        }
    }
    //m_generationThread = new std::thread(&BasicDungeonGenerator::stepMaze, this, Vec2(0, 0));
    stepMaze(Vec2(0, 0));

    std::uniform_int_distribution<int> roomDistribution(0, m_rooms.size()-1);
    Room currentRoom = m_rooms[roomDistribution(m_generator)];
    m_generationThread = new std::thread(&BasicDungeonGenerator::makeDoors, this, currentRoom);
}


void BasicDungeonGenerator::initCells(){
    for(int r = 0; r < m_wHeight; r++){
        m_game->getWorld().push_back(std::vector<Tile>());
        if(r % 2 == 1) m_visited.push_back(std::vector<CellData>());
        for(int c = 0; c < m_wWidth; c++){
            if(r % 2 == 1 && c % 2 == 1){
                m_game->getWorld().back().push_back(TileManager::floor);
                m_visited.back().push_back({false, -1});
            } else { 
                m_game->getWorld().back().push_back(TileManager::wall);
            }
        }
    }
}

void BasicDungeonGenerator::placeRoom(Room room){
    //need to convert rooms in cell space to rooms in world space
    for(int r = convToWorld(room.y, false); r < convToWorld(room.y + room.height); r++){
        for(int c = convToWorld(room.x, false); c < convToWorld(room.x + room.width); c++){
            if(c == convToWorld(room.x, false) || c == convToWorld(room.x + room.width, false) || r == convToWorld(room.y, false) || r == convToWorld(room.y + room.height, false)) {
                //this tile is a wall on the border of a room
                m_game->getWorld()[r][c] = TileManager::wall;
            } else {
                if(r % 2 == 1 && c % 2 == 1) m_visited[convToMaze(r)][convToMaze(c)] = {true, (int)m_rooms.size()};
                m_game->getWorld()[r][c] = TileManager::floor;
            }
        }
    }
    m_rooms.push_back(room);
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

void BasicDungeonGenerator::stepMaze(Vec2 cellPos){
    struct DirectionCell {
        char dir;
        Vec2 pos;

        DirectionCell(char dir, Vec2 pos) : dir(dir), pos(pos) {}
    };

    *getCell(cellPos) = {true, -1};

    std::vector<DirectionCell> neighbors;
    neighbors.reserve(4);
    //populate neighbors
    if(getNorth(cellPos) && !(getNorth(cellPos)->visited)) neighbors.emplace_back('N', cellPos + Vec2(-1, 0));
    if(getEast(cellPos)  &&  !(getEast(cellPos)->visited)) neighbors.emplace_back('E', cellPos + Vec2( 0, 1));
    if(getWest(cellPos)  &&  !(getWest(cellPos)->visited)) neighbors.emplace_back('W', cellPos + Vec2( 0,-1));
    if(getSouth(cellPos) && !(getSouth(cellPos)->visited)) neighbors.emplace_back('S', cellPos + Vec2( 1, 0)); 
    while(neighbors.size() > 0){
        //pick a neighbor 
        std::uniform_int_distribution<int> directonPicker(0, neighbors.size()-1);
        int selectedNum = directonPicker(m_generator);
        DirectionCell selected = neighbors[selectedNum];
        //cells visited status may have changed
        if(!(getCell(selected.pos)->visited)){
            //open the wall leading to the neighbor
            /*{
                std::unique_lock<std::mutex> lk(m_mtx);
                if(!m_shouldStep) m_cv.wait(lk);
                m_shouldStep = false;
            }*/

            switch(selected.dir){
                case 'N': *(getNorthWall(cellPos)) = TileManager::floor; break;
                case 'E': *(getEastWall(cellPos))  = TileManager::floor; break;
                case 'W': *(getWestWall(cellPos))  = TileManager::floor; break;
                case 'S': *(getSouthWall(cellPos)) = TileManager::floor; break;
            }
            stepMaze(selected.pos);
        }
        neighbors.erase(neighbors.begin()+selectedNum);
    }
}

void BasicDungeonGenerator::makeDoors(Room currentRoom){
    //if a room has already been merged have a random chance of adding another door
    //find a good connector from this room
    //break out of makingDoors if we have stumbled across all the rooms
    static int hiddenRooms = m_rooms.size() + 1;
    hiddenRooms--;
    if(hiddenRooms == 0) return;

    std::vector<ExplorationHead> edgeCells;
    std::vector<ExplorationHead> newEdgeCells;

    //TODO: find a better way to pick a random edge
    Vec2 inConn;
    while(true){
        std::uniform_int_distribution<int> rSide(0, 3), rWidth(0, currentRoom.width-1), rHeight(0, currentRoom.height-1);
        switch(rSide(m_generator)){
            case 0:
                inConn = {currentRoom.y, currentRoom.x + rWidth(m_generator)};
                if(!getNorth(inConn) || getNorth(inConn)->roomNumber != -1) continue;
                else edgeCells.push_back({inConn + Vec2(-1, 0), 'S'});
                *(getNorthWall(inConn)) = TileManager::floor;
                break;
            case 1:
                inConn = {currentRoom.y + rHeight(m_generator), currentRoom.x + currentRoom.width-1};
                if(!getEast(inConn) || getEast(inConn)->roomNumber != -1) continue;
                else edgeCells.push_back({inConn + Vec2( 0, 1), 'W'});
                *(getEastWall(inConn)) = TileManager::floor;
                break;
            case 2:
                inConn = {currentRoom.y + currentRoom.height-1, currentRoom.x + rWidth(m_generator)};
                if(!getSouth(inConn) || getSouth(inConn)->roomNumber != -1) continue;
                else edgeCells.push_back({inConn + Vec2( 1, 0), 'N'});
                *(getSouthWall(inConn)) = TileManager::floor;
                break;
            case 3:
                inConn = {currentRoom.y + rHeight(m_generator), currentRoom.x};
                if(!getWest(inConn) || getWest(inConn)->roomNumber != -1) continue;
                else edgeCells.push_back({inConn + Vec2( 0,-1), 'E'});
                *(getWestWall(inConn)) = TileManager::floor;
                break;
        }
        //make sure we don't link to a room directly (might change this)
        *(getWorld(inConn)) = TileManager::special;
        break;
    }
    //store the exploration in a tree until we reach the end destination and then decompose all the non terminal 
    //parts of the tree
    try {
        while(true){
            //explore the maze
            for(int i = 0; i < edgeCells.size(); i++){
                
                *(getWorld(edgeCells[i].pos)) = TileManager::special;

                //add new neighbors to newEdgeCells
                //check all of the neighbors around the current edge and see if any are rooms
                if(edgeCells[i].from != 'N') exploreMaze(edgeCells[i].pos, {-1, 0}, 'S', newEdgeCells);
                if(edgeCells[i].from != 'E') exploreMaze(edgeCells[i].pos, { 0, 1}, 'W', newEdgeCells);
                if(edgeCells[i].from != 'W') exploreMaze(edgeCells[i].pos, { 0,-1}, 'E', newEdgeCells);
                if(edgeCells[i].from != 'S') exploreMaze(edgeCells[i].pos, { 1, 0}, 'N', newEdgeCells);
            }

            {
                std::unique_lock<std::mutex> lk(m_mtx);
                if(!m_shouldStep) m_cv.wait(lk);
                m_shouldStep = false;
            }

            for(int i = 0; i < edgeCells.size(); i++){
                *(getWorld(edgeCells[i].pos)) = TileManager::floor;
            }

            edgeCells = newEdgeCells;
            newEdgeCells.clear();
        }
    } catch(const int roomNumber){
        makeDoors(m_rooms[roomNumber]);
    }
} 

void BasicDungeonGenerator::exploreMaze(Vec2 cell, Vec2 direction, char from, std::vector<ExplorationHead>& newEdgeCells){  
    Vec2 neighbor = cell + direction;
    CellData* neighborData = getCell(neighbor);
    Tile* neighborConnection = getWorld(cell, direction);
    if(neighborData){
        if(neighborData->roomNumber == -1) {
            //this is part of the maze
            if(*neighborConnection == TileManager::floor){
                newEdgeCells.push_back({neighbor, from});
            }
        } else if(!m_rooms[neighborData->roomNumber].merged){
            (*getWorld(cell, direction)) = TileManager::floor;
            m_rooms[neighborData->roomNumber].merged = true;
            throw neighborData->roomNumber;
        }
    }
}
