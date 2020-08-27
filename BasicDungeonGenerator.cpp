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
    Room& currentRoom = m_rooms[roomDistribution(m_generator)];
    currentRoom.merged = true; 
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

void BasicDungeonGenerator::makeDoors(Room currentRoom){
    //if a room has already been merged have a random chance of adding another door
    //find a good connector from this room
    //break out of makingDoors if we have stumbled across all the rooms
    static int hiddenRooms = m_rooms.size();
    hiddenRooms--;
    if(hiddenRooms == 0) return;

    std::vector<ExplorationNode*> edgeCells;
    std::vector<ExplorationNode*> newEdgeCells;
    ExplorationNode* root;

    std::vector<ExplorationNode*> deadEnds;

    //TODO: make a more bare bones switch case and put the rest of the stuff in a different spot
    Vec2 inConn;
    while(true){
        std::uniform_int_distribution<int> rSide(0, 3), rWidth(0, currentRoom.width-1), rHeight(0, currentRoom.height-1);
        switch(rSide(m_generator)){
            case 0:
                inConn = {currentRoom.y, currentRoom.x + rWidth(m_generator)};
                if(!getCell(inConn + north) || getCell(inConn + north)->roomNumber != -1) continue;
                else root = new ExplorationNode(inConn + north, south, nullptr);
                *(getWorld(inConn, north)) = TileManager::floor;
                break;
            case 1:
                inConn = {currentRoom.y + rHeight(m_generator), currentRoom.x + currentRoom.width-1};
                if(!getCell(inConn + east) || getCell(inConn + east)->roomNumber != -1) continue;
                else root = new ExplorationNode(inConn + east, west, nullptr);
                *(getWorld(inConn, east)) = TileManager::floor;
                break;
            case 2:
                inConn = {currentRoom.y + currentRoom.height-1, currentRoom.x + rWidth(m_generator)};
                if(!getCell(inConn + south) || getCell(inConn + south)->roomNumber != -1) continue;
                else root = new ExplorationNode(inConn + south, north, nullptr);
                *(getWorld(inConn, south)) = TileManager::floor;
                break;
            case 3:
                inConn = {currentRoom.y + rHeight(m_generator), currentRoom.x};
                if(!getCell(inConn + west) || getCell(inConn + west)->roomNumber != -1) continue;
                else root = new ExplorationNode(inConn + west, east, nullptr);
                *(getWorld(inConn, west)) = TileManager::floor;
                break;
        }
        break;
    }
    *(getWorld(inConn)) = TileManager::special;
    edgeCells.push_back(root);
    getCell(root->pos)->entrance = true;

    //store the exploration in a tree until we reach the end destination and then decompose all the non terminal 
    //parts of the tree
    try {
        while(true){
            //explore the maze
            /*for(int i = 0; i < root->children.size(); i++){
                std::cout << "child " << i << ": " << &(root->children[i]) << std::endl;
            }*/
            for(int i = 0; i < edgeCells.size(); i++){
                //std::cout << "edgeCell " << i << ": " << edgeCells[i] << std::endl;

                *(getWorld(edgeCells[i]->pos)) = TileManager::special;

                //add new neighbors to newEdgeCells
                //check all of the neighbors around the current edge and see if any are rooms
                edgeCells[i]->children.reserve(4);
                if(edgeCells[i]->from != north) exploreMaze(edgeCells[i], north, newEdgeCells);
                if(edgeCells[i]->from != east) exploreMaze(edgeCells[i], east,  newEdgeCells);
                if(edgeCells[i]->from != west) exploreMaze(edgeCells[i], west,  newEdgeCells);
                if(edgeCells[i]->from != south) exploreMaze(edgeCells[i], south, newEdgeCells);

                if(edgeCells[i]->children.size() == 0 && !getCell(edgeCells[i]->pos)->entrance) deadEnds.push_back(edgeCells[i]);
            }

            //wait for the space bar to be pressed
            {
                std::unique_lock<std::mutex> lk(m_mtx);
                if(!m_shouldStep) m_cv.wait(lk);
                m_shouldStep = false;
            }

            for(int i = 0; i < edgeCells.size(); i++){
                *(getWorld(edgeCells[i]->pos)) = TileManager::floor;
            }

            edgeCells = newEdgeCells;
            newEdgeCells.clear();
        }
    } catch(const int roomNumber){
        //prune the tree and delete it
        for(int i = 0; i < deadEnds.size(); i++){
            ExplorationNode* currNode = deadEnds[i];
            while(true){
                /*if(currNode->parent->children.size() > 1 || getCell(currNode->parent->pos)->entrance) {
                    *(getWorld(currNode->pos, currNode->from)) = TileManager::wall;
                    break;
                }*/
                if(currNode->children.size() > 1 || getCell(currNode->pos)->entrance) break;
                *(getWorld(currNode->pos)) = TileManager::wall;
                *(getWorld(currNode->pos, currNode->from)) = TileManager::wall;
                currNode = currNode->parent;
            }
        }
        delete root;
        makeDoors(m_rooms[roomNumber]);
    }
}

void BasicDungeonGenerator::exploreMaze(ExplorationNode* node, Vec2 direction, std::vector<ExplorationNode*>& newEdgeCells){  
    Vec2 neighborPos = node->pos + direction;
    CellData* neighborData = getCell(neighborPos);
    Tile* neighborConnection = getWorld(node->pos, direction);
    if(neighborData){
        if(neighborData->roomNumber == -1) {
            //this is part of the maze
            if(*neighborConnection == TileManager::floor){
                ExplorationNode& child = node->createChild(neighborPos, direction*-1);
                newEdgeCells.push_back(&child);
            }
        } else if(!m_rooms[neighborData->roomNumber].merged){
            //cannot possibly already be a door
            getCell(node->pos)->entrance = true;
            (*getWorld(node->pos, direction)) = TileManager::floor;
            m_rooms[neighborData->roomNumber].merged = true;
            throw neighborData->roomNumber;
        }
    }
}
