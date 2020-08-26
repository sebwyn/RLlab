#pragma once

#include <random>
#include <utility>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <optional>

#include "Component.hpp"
#include "RoguelikeGame.hpp"

class BasicDungeonGenerator : public Component {
public:
    //width and height in cells, a cell is surrounded by 8 wall characters
    BasicDungeonGenerator(RoguelikeGame* game, int width, int height, int seed, int roomAttempts, int minRoomSize, int maxRoomSize) 
     :  m_game(game), m_width(width), m_height(height), m_wWidth(width*2+1), m_wHeight(height*2+1), m_attempts(roomAttempts), 
        m_sizeDistribution(minRoomSize, maxRoomSize), m_colDistribution(0, width), m_rowDistribution(0, height) 
    { 
        m_generator.seed(seed);
        generate();
    }

    virtual void update() override;
    virtual void handleInput(int key) override;
private:
    RoguelikeGame* m_game;
    int m_width, m_height, m_wWidth, m_wHeight;

    std::default_random_engine m_generator;
    std::uniform_int_distribution<int> m_sizeDistribution, m_colDistribution, m_rowDistribution;
    
    struct Room {
        int x, y, width, height;
        bool merged;
    };

    struct CellData {
        bool visited;
        int roomNumber;
    };

    //maze space primarily
    struct Vec2 {
        int r, c;
        
        Vec2() = default;
        Vec2(int r, int c) : r(r), c(c) {}

        Vec2 operator + (const Vec2& other){
            return {r + other.r, c + other.c};
        }
    };

    struct ExplorationHead {
        Vec2 pos;
        char from;
    };
    
    int m_attempts; 
    std::vector<Room> m_rooms;
    std::vector<std::vector<CellData>> m_visited; //visited cells either by a room or maze

    bool m_shouldStep = false;
    std::condition_variable m_cv;
    std::mutex m_mtx;
    std::thread* m_generationThread;
    
    CellData* getCell(Vec2 cell){
        if(0 <= cell.r && cell.r < m_height && 0 <= cell.c && cell.c < m_width) 
            return &(m_visited[cell.r][cell.c]);
        else return nullptr;
    }
    inline CellData* getNorth(Vec2 cell){return getCell(cell + Vec2(-1, 0)); }
    inline CellData* getEast(Vec2 cell) {return getCell(cell + Vec2( 0, 1)); }
    inline CellData* getWest(Vec2 cell) {return getCell(cell + Vec2( 0,-1)); }
    inline CellData* getSouth(Vec2 cell){return getCell(cell + Vec2( 1, 0)); }

    inline Tile* getWorld(Vec2 cell, Vec2 direction = Vec2(0, 0)){return &(m_game->getWorld()[convToWorld(cell.r)+direction.r][convToWorld(cell.c)+direction.c]);}
    inline Tile* getNorthWall(Vec2 cell){return getWorld(cell, Vec2(-1, 0)); }
    inline Tile* getEastWall(Vec2 cell) {return getWorld(cell, Vec2( 0, 1)); }
    inline Tile* getWestWall(Vec2 cell) {return getWorld(cell, Vec2( 0,-1)); }
    inline Tile* getSouthWall(Vec2 cell){return getWorld(cell, Vec2( 1, 0)); }
    
    int convToWorld(int pos, bool aligned = true){
        return pos * 2 + (aligned ? 1 : 0);
    }
    int convToMaze(int pos){
        return pos / 2;
    }

    void generate();

    void initCells();
    bool validateRoom(Room room);
    void placeRoom(Room room);
    void stepMaze(Vec2 cell);
    void makeDoors(Room currentRoom);
    void exploreMaze(Vec2 cell, Vec2 direction, char from, std::vector<ExplorationHead>& newEdgeCells);
};
