#pragma once

#include <random>
#include <utility>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <optional>

#include "Vec2.hpp"
#include "Tile.hpp"

class BasicDungeonGenerator {
public:
    //width and height in cells, a cell is surrounded by 8 wall characters
    BasicDungeonGenerator(std::vector<std::vector<Tile>>& worldData, int x, int y, int seed, int roomAttempts, int minRoomSize, int maxRoomSize) 
     : m_worldData(worldData), 
       m_x(x), 
       m_y(y),
       m_wX(x*2+1), 
       m_wY(y*2+1), 
       m_attempts(roomAttempts), 
       m_minRoomSize(minRoomSize),
       m_maxRoomSize(maxRoomSize)
    { 
        m_generator.seed(seed);
        generate();
    }

    Vec2 getSpawnPoint();
private:
    std::vector<std::vector<Tile>>& m_worldData;
    int m_x, m_y, m_wX, m_wY, m_minRoomSize, m_maxRoomSize;

    std::default_random_engine m_generator;
    
    struct Room {
        Vec2 pos, size;
    };

    struct CellData {
        bool visited;
        int roomNumber;
        bool entrance = false;
    };

    struct ExplorationNode {
        Vec2 pos;
        Vec2 from;
        ExplorationNode* parent;
        std::vector<ExplorationNode> children;

        bool deadEnd = false;

        ExplorationNode() = default;
        ExplorationNode(Vec2 pos, Vec2 from, ExplorationNode* parent) 
         : pos(pos), 
           from(from), 
           parent(parent) 
        {}
        
        ExplorationNode& createChild(Vec2 cPos, Vec2 cFrom)
        {
            children.emplace_back(cPos, cFrom, this);
            return children.back();
        }

        bool operator == (const ExplorationNode& other)
        {
            if(pos == other.pos) return true;
            return false;
        }
    };

    int m_attempts; 
    std::vector<Room> m_rooms;

    //visited cells either by a room or maze
    std::vector<std::vector<CellData>> m_cells; 

    CellData* getCell(Vec2 cell){
        if(0 <= cell.x && cell.x < m_x && 0 <= cell.y && cell.y < m_y) 
            return &(m_cells[cell.x][cell.y]);
        else return nullptr;
    }

    inline Tile* getWorld(Vec2 cell, Vec2 offset = Vec2(0, 0)){
        Vec2 worldP = convToWorld(cell) + offset;
        return &(m_worldData[worldP.x][worldP.y]);
    }
    
    inline int convToWorld(int pos, bool aligned = true){
        return pos * 2 + (aligned ? 1 : 0);
    }
    inline Vec2 convToWorld(Vec2 pos, bool aligned = true){
        return pos * 2 + (aligned ? Vec2(1, 1) : Vec2(0, 0));
    }
    inline int convToMaze(int pos){
        return pos / 2;
    }
    inline Vec2 convToMaze(Vec2 pos){
        return pos / 2;
    }
    
    int random(int start, int end){
        std::uniform_int_distribution<int> distribution(start, end);
        return distribution(m_generator);
    }

    void generate();

    void clearData();
    void initCells();
    bool validateRoom(Room room);
    void placeRoom(Room room);
    void stepMaze(Vec2 cell);
    void makeDoors(Room currentRoom, int numOfDoors = 1);
    bool placeDoor(Vec2 position, Vec2 direction);
    void exploreMaze();
    void stepExploration(ExplorationNode* node, Vec2 direction, std::vector<ExplorationNode*>& newEdgeCells);
    void handleDeadEnd(ExplorationNode* deadEnd);
};
