#include "BasicDungeonGenerator.hpp"

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
        case 'b':
            {
                int a = 10;
            }
            break;
    }
}

void BasicDungeonGenerator::generate(){
    initCells();
    for(int i = 0; i < m_attempts; i++){
        Room room = {m_colDistribution(m_generator), m_rowDistribution(m_generator), m_sizeDistribution(m_generator), m_sizeDistribution(m_generator)}; 
        if(validateRoom(room)){
            placeRoom(room);
        }
    }
    //m_generationThread = new std::thread(&BasicDungeonGenerator::stepMaze, this, Vec2(0, 0));
    stepMaze(Vec2(0, 0));
    //wait for the space bar to be pressed
    {
        std::unique_lock<std::mutex> lk(m_mtx);
        if(!m_shouldStep) m_cv.wait(lk);
        m_shouldStep = false;
    }
    std::uniform_int_distribution<int> numDoorRange(1, 3);
    for(int i = 0; i < m_rooms.size(); i++){
        int numDoors = numDoorRange(m_generator); 
        makeDoors(m_rooms[i], numDoors);
    }
    exploreMaze();
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
    if(1 < room.x && room.x + room.width < m_width-1 && 1 < room.y && room.y + room.height < m_height-1){
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
    if(getCell(cellPos + north) && !getCell(cellPos + north)->visited) neighbors.emplace_back('N', cellPos + north);
    if(getCell(cellPos + east)  && !getCell(cellPos + east)->visited)  neighbors.emplace_back('E', cellPos + east);
    if(getCell(cellPos + west)  && !getCell(cellPos + west)->visited)  neighbors.emplace_back('W', cellPos + west);
    if(getCell(cellPos + south) && !getCell(cellPos + south)->visited) neighbors.emplace_back('S', cellPos + south); 
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
                case 'N': *(getWorld(cellPos, north)) = TileManager::floor; break;
                case 'E': *(getWorld(cellPos, east))  = TileManager::floor; break;
                case 'W': *(getWorld(cellPos, west))  = TileManager::floor; break;
                case 'S': *(getWorld(cellPos, south)) = TileManager::floor; break;
            }
            stepMaze(selected.pos);
        }
        neighbors.erase(neighbors.begin()+selectedNum);
    }
}

void BasicDungeonGenerator::exploreMaze(){

    std::vector<ExplorationNode*> edgeCells;
    std::vector<ExplorationNode*> newEdgeCells;
    ExplorationNode* root;

    std::vector<ExplorationNode*> deadEnds;
    
    //naively search through cells until we find an entrance
    for(int r = 0; r < m_height; r++){
        for(int c = 0; c < m_width; c++){
            if(getCell(Vec2(r, c))->entrance) root = new ExplorationNode(Vec2(r, c), Vec2(0, 0), nullptr);
        } 
    }
    edgeCells.push_back(root);

    //store the exploration in a tree until we reach the end destination and then decompose all the non terminal 
    //parts of the tree
    while(true){
        //explore the maze
        if(edgeCells.size() == 0) break;
        for(int i = 0; i < edgeCells.size(); i++){

            *(getWorld(edgeCells[i]->pos)) = TileManager::special;

            //add new neighbors to newEdgeCells
            //check all of the neighbors around the current edge and see if any are rooms
            edgeCells[i]->children.reserve(4);
            if(edgeCells[i]->from != north) stepExploration(edgeCells[i], north, newEdgeCells);
            if(edgeCells[i]->from != east)  stepExploration(edgeCells[i], east,  newEdgeCells);
            if(edgeCells[i]->from != west)  stepExploration(edgeCells[i], west,  newEdgeCells);
            if(edgeCells[i]->from != south) stepExploration(edgeCells[i], south, newEdgeCells);

            if(edgeCells[i]->children.size() == 0 && !getCell(edgeCells[i]->pos)->entrance){
                ExplorationNode *currNode = edgeCells[i];
                while(true){
                    *(getWorld(currNode->pos)) = TileManager::wall;
                    *(getWorld(currNode->pos, currNode->from)) = TileManager::wall;
                    //possibly deleting the child at the intersection instead of the one next to the intersection 
                    int aliveSiblings = 0;
                    for(int s = 0; s < currNode->parent->children.size(); s++)
                        if(!(currNode->parent->children[s].deadEnd)) aliveSiblings++;
                    if(aliveSiblings > 1 || getCell(currNode->parent->pos)->entrance) {
                        currNode->deadEnd = true;
                        break;
                    }
                    else currNode = currNode->parent;
                }
            }
        }

        //wait for the space bar to be pressed
        {
            std::unique_lock<std::mutex> lk(m_mtx);
            if(!m_shouldStep) m_cv.wait(lk);
            m_shouldStep = false;
        }

        edgeCells = newEdgeCells;
        newEdgeCells.clear();
    }
    delete root;
}

void BasicDungeonGenerator::stepExploration(ExplorationNode* node, Vec2 direction, std::vector<ExplorationNode*>& newEdgeCells){  
    Vec2 neighborPos = node->pos + direction;
    CellData* neighborData = getCell(neighborPos);
    Tile* neighborConnection = getWorld(node->pos, direction);
    //if the neighbor cell exists and is part of the maze (not a room)
    if(neighborData && neighborData->roomNumber == -1){
        if(*neighborConnection == TileManager::floor){
            ExplorationNode& child = node->createChild(neighborPos, direction*-1);
            newEdgeCells.push_back(&child);
        }
    }
}

bool BasicDungeonGenerator::placeDoor(Vec2 position, Vec2 direction){
    CellData* doorData = getCell(position + direction);
    if (doorData && doorData->roomNumber == -1 && !doorData->entrance){
        *(getWorld(position, direction)) = TileManager::floor;
        return true;
    } else return false;
}

void BasicDungeonGenerator::makeDoors(Room currentRoom, int numOfDoors){
    //TODO: make a more bare bones switch case and put the rest of the stuff in a different spot
    std::uniform_int_distribution<int> rSide(0, 3), rWidth(0, currentRoom.width-1), rHeight(0, currentRoom.height-1);
    Vec2 inConn, direction;
    int doorsAdded = 0;
    while(doorsAdded != numOfDoors){
        switch(rSide(m_generator)){
            case 0:
                inConn = {currentRoom.y, currentRoom.x + rWidth(m_generator)};
                direction = north;
                break;
            case 1:
                inConn = {currentRoom.y + rHeight(m_generator), currentRoom.x + currentRoom.width-1};
                direction = east;
                break;
            case 2:
                inConn = {currentRoom.y + currentRoom.height-1, currentRoom.x + rWidth(m_generator)};
                direction = south;
                break;
            case 3:
                inConn = {currentRoom.y + rHeight(m_generator), currentRoom.x};
                direction = west;
                break;
        }
        if(placeDoor(inConn, direction)){
            (getCell(inConn + direction))->entrance = true;
            doorsAdded++;
        }
    }
    //*(getWorld(inConn)) = TileManager::special;
}

