#pragma once

#include "Monarch.hpp"

#include "BasicDungeonGenerator.hpp"
#include "Tile.hpp"

class World : public Monarch::Component {
public:
    World(int width, int height, int seed);

    virtual void init() override;
    virtual void update() override;
    virtual void draw() override; 

    Vec2 getSpawnPoint();

    inline int getTileSize() const { return m_tileSize; }
    inline const std::vector<std::vector<Tile>>& getData() const { return m_data; }

    virtual bool onEvent(Monarch::Event& e) override;

    Tile* getTile(Vec2 worldPos){
        if(0 <= worldPos.x && worldPos.x < m_width && 0 <= worldPos.y && worldPos.y < m_height) 
            return &(m_data[worldPos.x][worldPos.y]);
        else return nullptr;
    }
private:
    int m_width, m_height;

    int m_tileSize = 10;
    std::vector<std::vector<Tile>> m_data;
    BasicDungeonGenerator m_generator; 
};
