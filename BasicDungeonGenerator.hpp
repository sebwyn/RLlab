#pragma once

#include <random>
#include <utility>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "LevelGenerator.hpp"
#include "Component.hpp"
#include "RoguelikeGame.hpp"

class BasicDungeonGenerator : public LevelGenerator, public Component {
public:
    BasicDungeonGenerator(int width, int height, int seed, int roomAttempts, int minRoomSize, int maxRoomSize) 
     : LevelGenerator(width, height), m_attempts(roomAttempts), m_sizeDistribution(minRoomSize, maxRoomSize), m_colDistribution(0, width), m_rowDistribution(0, height) { 
        generator.seed(seed);
    }

    BasicDungeonGenerator(RoguelikeGame* game, int seed, int roomAttempts, int minRoomSize, int maxRoomSize) 
     : LevelGenerator(game->getWidth(), game->getHeight()), m_attempts(roomAttempts), m_sizeDistribution(minRoomSize, maxRoomSize), m_colDistribution(0, game->getWidth()), m_rowDistribution(0, game->getHeight()) { 
        generator.seed(seed);
        generate(game->getWorld());
    }

    virtual void update() override;
    virtual void handleInput(int key) override;
    virtual void generate(std::vector<std::vector<Tile>>& tiles) override;
private:
    struct Room {
        int x, y, width, height;
        Room(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}
    };

    std::default_random_engine generator;
    std::vector<Room> m_rooms;
    int m_attempts; 
    std::uniform_int_distribution<int> m_sizeDistribution, m_colDistribution, m_rowDistribution; 
    
    std::mutex mtx;
    std::condition_variable cv;
    std::vector<std::vector<bool>> visited;
    bool shouldStepMaze = false, finishedMaze = false;
    std::thread* mazeGenerator;

    bool validateRoom(Room room);
    void placeRoom(Room room);
    void generateMaze();
    void stepMaze(int cr, int cc);
};
