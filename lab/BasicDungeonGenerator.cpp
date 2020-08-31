#include "BasicDungeonGenerator.hpp"

/*
 * set the player spawn to be the upper left corner of the first room
 */
Vec2 BasicDungeonGenerator::getSpawnPoint()
{
    return convToWorld(m_rooms[0].pos);
}

/*
 * initialize the world data to the proper format
 * attempt to place m_attempts rooms randomly
 * flood fill the area surrounding the rooms with maze
 * randomly make doors in all the rooms
 * explore the maze and get rid of dead ends
 */
void BasicDungeonGenerator::generate()
{
    clearData();
    initCells();
    for(int i = 0; i < m_attempts; i++){
        Room room = {
            Vec2(random(1, m_x-1), random(1, m_y-1)), 
            Vec2(random(m_minRoomSize, m_maxRoomSize),random(m_minRoomSize, m_maxRoomSize))
        }; 
        if(validateRoom(room)){
            m_rooms.push_back(room);
            placeRoom(room);
        }
    }
    stepMaze(Vec2(0, 0));
    for(int i = 0; i < m_rooms.size(); i++){
        int numDoors = random(1, 3); 
        makeDoors(m_rooms[i], numDoors);
    }
    exploreMaze();
}

void BasicDungeonGenerator::clearData()
{
    m_worldData.clear();
    m_cells.clear();
}

/*
 * populate the world data with the proper formatted cells for this type of 
 * generation
 * while populating m_cells
 */
void BasicDungeonGenerator::initCells()
{
    for(int r = 0; r < m_wX; r++){
        m_worldData.push_back(std::vector<Tile>());
        if(r % 2 == 1) m_cells.push_back(std::vector<CellData>());
        for(int c = 0; c < m_wY; c++){
            if(r % 2 == 1 && c % 2 == 1){
                m_worldData.back().push_back(TileManager::floor);
                m_cells.back().push_back({false, -1});
            } else { 
                m_worldData.back().push_back(TileManager::wall);
            }
        }
    }
}

/* 
 * change the world data at the room location 
 * to make the room appear in the world
 * assuming that initCells has already been called
 */
void BasicDungeonGenerator::placeRoom(Room room)
{
    //iterate over the inside of the room in world space clearing it out
    //and if the current pos in world space is a cell, make it visited
    Vec2 topLeft = convToWorld(Vec2(room.pos.x, room.pos.y));
    Vec2 bottomRight = topLeft + convToWorld(Vec2(room.size.x-1, room.size.y-1)); 
    for(int r = topLeft.x; r < bottomRight.x; r++){
        for(int c = topLeft.y; c < bottomRight.y; c++){
            if(r % 2 == 1 && c % 2 == 1)
                *(getCell(convToMaze(Vec2(r, c)))) = {true, (int)m_rooms.size()};
            m_worldData[r][c] = TileManager::floor;
        }
    }
}

//might have some collision problems
bool BasicDungeonGenerator::validateRoom(Room room)
{
    Vec2 roomStart = room.pos;
    Vec2 roomExtent = roomStart + room.size;
    //check to make sure the room is in the world bounds
    if(!(0 < roomStart.x && roomExtent.x < m_x 
    &&   0 < roomStart.y && roomExtent.y < m_y))
        return false;

    //check to make sure the room doesn't collide with any other rooms
    for(Room r : m_rooms){
        Vec2 otherStart = r.pos;
        Vec2 otherExtent = otherStart + r.size;
        if((otherStart.x <= room.pos.x && room.pos.x < otherExtent.x) 
        || (roomStart.x <= otherStart.x && otherStart.x < roomExtent.x)){
            if((otherStart.y <= roomStart.y && roomStart.y < otherExtent.y) 
            || (roomStart.y <= otherStart.y && otherStart.y < roomExtent.y)){
                return false;
            }
        }
    }
    return true;
}

/*
 * this is the function that actually flood fills a maze
 * using recursive backtracking
 */
void BasicDungeonGenerator::stepMaze(Vec2 cellPos)
{
    struct DirectionCell {
        char dir;
        Vec2 pos;

        DirectionCell(char dir, Vec2 pos) 
         : dir(dir), 
           pos(pos) 
        {}
    };

    getCell(cellPos)->visited = true;

    std::vector<DirectionCell> neighbors;
    neighbors.reserve(4);
    //populate neighbors
    if(getCell(cellPos + north) && !getCell(cellPos + north)->visited) 
        neighbors.emplace_back('N', cellPos + north);
    if(getCell(cellPos + east)  && !getCell(cellPos + east)->visited)  
        neighbors.emplace_back('E', cellPos + east);
    if(getCell(cellPos + west)  && !getCell(cellPos + west)->visited)  
        neighbors.emplace_back('W', cellPos + west);
    if(getCell(cellPos + south) && !getCell(cellPos + south)->visited) 
        neighbors.emplace_back('S', cellPos + south); 
    while(neighbors.size() > 0){
        //pick a neighbor 
        int selectedNum = random(0, neighbors.size()-1);
        DirectionCell selected = neighbors[selectedNum];
        //cells visited status may have changed
        if(!(getCell(selected.pos)->visited)){
            //open the wall leading to the neighbor
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

/*
 * explore every part of the maze and fill in dead ends as it goes
 */
void BasicDungeonGenerator::exploreMaze()
{
    std::vector<ExplorationNode*> edgeCells;
    std::vector<ExplorationNode*> newEdgeCells;
    ExplorationNode* root;

    std::vector<ExplorationNode*> deadEnds;
    
    //naively search through cells until we find an entrance
    for(int r = 0; r < m_x; r++){
        for(int c = 0; c < m_y; c++){
            if(getCell(Vec2(r, c))->entrance)
                root = new ExplorationNode(Vec2(r, c), Vec2(0, 0), nullptr);
        } 
    }
    edgeCells.push_back(root);

    //store the exploration in a tree until we reach the end destination and 
    //then decompose all the non terminal parts of the tree
    while(true){
        //explore the maze
        if(edgeCells.size() == 0) break;
        for(int i = 0; i < edgeCells.size(); i++){
            //check all of the neighbors around the current edge and see if any are rooms
            //add new neighbors to newEdgeCells
            edgeCells[i]->children.reserve(4);
            if(edgeCells[i]->from != north) 
                stepExploration(edgeCells[i], north, newEdgeCells);
            if(edgeCells[i]->from != east)  
                stepExploration(edgeCells[i], east,  newEdgeCells);
            if(edgeCells[i]->from != west)  
                stepExploration(edgeCells[i], west,  newEdgeCells);
            if(edgeCells[i]->from != south) 
                stepExploration(edgeCells[i], south, newEdgeCells);

            handleDeadEnd(edgeCells[i]);
        }
        edgeCells = newEdgeCells;
        newEdgeCells.clear();
    }
    delete root;
}

void BasicDungeonGenerator::stepExploration(ExplorationNode* node, Vec2 direction, std::vector<ExplorationNode*>& newEdgeCells)
{  
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

bool BasicDungeonGenerator::placeDoor(Vec2 position, Vec2 direction)
{
    CellData* doorData = getCell(position + direction);
    if (doorData && doorData->roomNumber == -1 && !doorData->entrance){
        *(getWorld(position, direction)) = TileManager::floor;
        return true;
    } else return false;
}

void BasicDungeonGenerator::makeDoors(Room currentRoom, int numOfDoors)
{
    //TODO: make a more bare bones switch case and put the rest of the stuff in a different spot
    Vec2 inConn, direction;
    int doorsAdded = 0;
    while(doorsAdded != numOfDoors){
        switch(random(0, 3)){
            case 0:
                inConn = {
                    currentRoom.pos.x,
                    currentRoom.pos.y + random(0, currentRoom.size.y-1)
                };
                direction = north;
                break;
            case 1:
                inConn = {
                    currentRoom.pos.x + random(0, currentRoom.size.x-1),
                    currentRoom.pos.y + currentRoom.size.y-1
                };
                direction = east;
                break;
            case 2:
                inConn = {
                    currentRoom.pos.x + currentRoom.size.x-1,
                    currentRoom.pos.y + random(0, currentRoom.size.y-1)
                };
                direction = south;
                break;
            case 3:
                inConn = {
                    currentRoom.pos.x + random(0, currentRoom.size.x-1),
                    currentRoom.pos.y
                };
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

void BasicDungeonGenerator::handleDeadEnd(ExplorationNode* node)
{
    //check to see if this cell is a dead end and march up the tree
    //filling in the maze until we reach an intersection
    if(node->children.size() == 0 && !getCell(node->pos)->entrance){
        ExplorationNode *currNode = node;
        while(true){
            //fill in the world with walls
            *(getWorld(currNode->pos)) = TileManager::wall;
            *(getWorld(currNode->pos, currNode->from)) = TileManager::wall;
            //check to see if the parent of the current node is an intersection
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
