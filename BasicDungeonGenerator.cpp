#include "BasicDungeonGenerator.hpp"

#include <tuple>

void BasicDungeonGenerator::update(){
    static bool joined = false;
    if(!joined && finishedMaze){
        mazeGenerator->join();
        delete mazeGenerator;
        joined = true;
    }
}

void BasicDungeonGenerator::handleInput(int key){
    switch(key){
        case(' '): {
            if(!finishedMaze){
                std::unique_lock<std::mutex> lck(mtx);
                shouldStepMaze = true;
                cv.notify_one();
            }
        }
    }
}

void BasicDungeonGenerator::generate(std::vector<std::vector<Tile>>& tiles){
    m_tiles = &tiles;
    populateTiles(' ');
    /*for(int i = 0; i < m_attempts; i++){
        Room room = Room(m_colDistribution(generator), m_rowDistribution(generator), m_sizeDistribution(generator), m_sizeDistribution(generator)); 
        if(validateRoom(room)){
            placeRoom(room);
            m_rooms.push_back(room);
        }
    }*/
    generateMaze();
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
    for(int r = room.y; r < room.y + room.height; r++){
        for(int c = room.x; c < room.x + room.width; c++){
            if(c == room.x || c == room.x + room.width-1 || r == room.y || r == room.y + room.height-1){
                //this tile is a wall on the border of a room
                (*m_tiles)[r][c] = Tile('#');
            } else {
                (*m_tiles)[r][c] = Tile('.');
            }
        }
    }
}

void BasicDungeonGenerator::generateMaze(){
    //generate all the cells and walls 
    for(int r = 0; r < m_width; r++){
        if(r % 2 == 1) visited.push_back(std::vector<bool>());
        for(int c = 0; c < m_height; c++){
            if(r % 2 == 1 && c % 2 == 1){
                (*m_tiles)[r][c] = Tile(' ');
                visited.back().push_back(false);
            } else { 
                (*m_tiles)[r][c] = Tile('#');
            }
        }
    }
    mazeGenerator = new std::thread(&BasicDungeonGenerator::stepMaze, this, 0, 0);
}

void BasicDungeonGenerator::stepMaze(int cr, int cc){
    struct DirectionCell {
        char dir;
        int r, c;
    };

    visited[cr][cc] = true;
    std::vector<DirectionCell> neighbors;
    neighbors.reserve(4);
    //populate neighbors
    if(cr > 0            && !visited[cr-1][cc]) neighbors.push_back({'N', cr-1, cc});
    if(cc > 0            && !visited[cr][cc-1]) neighbors.push_back({'W', cr, cc-1});
    if(cc+1 < m_width/2  && !visited[cr][cc+1]) neighbors.push_back({'E', cr, cc+1});
    if(cr+1 < m_height/2 && !visited[cr+1][cc]) neighbors.push_back({'S', cr+1, cc});  
    while(neighbors.size() > 0){
        //pick a neighbor 
        std::uniform_int_distribution<int> directonPicker(0, neighbors.size()-1);
        int selectedNum = directonPicker(generator);
        DirectionCell selected = neighbors[selectedNum]; 
        //open the wall leading to the neighbor
        //cells visited status may have changed
        if(!visited[selected.r][selected.c]){
            //wait until the controlling process wants us to step
            /*{
                std::unique_lock<std::mutex> lck(mtx);
                while(!shouldStepMaze) cv.wait(lck);
                shouldStepMaze = false;
            }*/
            int worldRow = cr*2+1; int worldCol = cc*2+1;
            switch(selected.dir){
                case 'N': (*m_tiles)[worldRow-1][worldCol] = Tile(' '); break;
                case 'W': (*m_tiles)[worldRow][worldCol-1] = Tile(' '); break;
                case 'E': (*m_tiles)[worldRow][worldCol+1] = Tile(' '); break;
                case 'S': (*m_tiles)[worldRow+1][worldCol] = Tile(' '); break;
            }
            stepMaze(selected.r, selected.c);
        }
        neighbors.erase(neighbors.begin()+selectedNum);
    }
    if(cr == 0 && cc == 0) finishedMaze = true;
}
