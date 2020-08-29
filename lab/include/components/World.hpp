#pragma once

#include "Monarch.hpp"

#include "BasicDungeonGenerator.hpp"
#include "Tile.hpp"

class World : public Monarch::Component {
public:
    World(int width, int height);

    virtual void init() override;
    virtual void update() override;
    virtual void draw() override; 

    virtual bool onEvent(Monarch::Event& e) override;
private:
    int m_tileSize = 10;
    std::vector<std::vector<Tile>> m_data;
    BasicDungeonGenerator m_generator; 
};
