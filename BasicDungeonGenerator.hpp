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
     :  m_game(game), m_width(width), m_height(height), m_wWidth(width*2+1), m_wHeight(height*2+1), m_attempts(roomAttempts), m_sizeDistribution(minRoomSize, maxRoomSize), m_colDistribution(0, width), m_rowDistribution(0, height) { 
        m_generator.seed(seed);
        generate();
    }

    virtual void update() override;
    virtual void handleInput(int key) override;
private:
    struct Room {
        int x, y, width, height;
        bool merged;
    };
    RoguelikeGame* m_game;
    int m_width, m_height, m_wWidth, m_wHeight;

    std::default_random_engine m_generator;
    std::uniform_int_distribution<int> m_sizeDistribution, m_colDistribution, m_rowDistribution; 

    struct DirectionCell {
        char dir;
        int r, c;
    };
    
    struct CellData {
        bool visited;
        int region;
    };

    int m_currentRegion = 0;
    int m_attempts; 
    std::vector<Room> m_rooms;
    std::vector<std::vector<CellData>> m_visited; //visited cells either by a room or maze
    std::vector<int> connectedRegions;
    
    std::optional<CellData> lookNorth(int cr, int cc);
    std::optional<CellData> lookEast (int cr, int cc);
    std::optional<CellData> lookSouth(int cr, int cc);
    std::optional<CellData> lookWest (int cr, int cc);

    void generate();

    void initCells();
    bool validateRoom(Room room);
    void placeRoom(Room room);
    void stepMaze(int cr, int cc);
    void makeDoors(Room currentRoom);
    void makePassage(int cr, int cc);
};
