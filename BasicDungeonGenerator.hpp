#pragma once

#include <random>
#include <utility>
#include <thread>
#include <mutex>
#include <condition_variable>

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
    };
    RoguelikeGame* m_game;
    int m_width, m_height, m_wWidth, m_wHeight;

    std::default_random_engine m_generator;
    std::uniform_int_distribution<int> m_sizeDistribution, m_colDistribution, m_rowDistribution; 

    int m_attempts; 
    std::vector<Room> m_rooms;
    std::vector<std::vector<bool>> m_visited; //visited cells either by a room or maze
    
    void generate();

    void initCells();
    bool validateRoom(Room room);
    void placeRoom(Room room);
    void generateMaze();
    void stepMaze(int cr, int cc);
};
