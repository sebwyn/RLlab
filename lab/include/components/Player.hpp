#pragma once

#include "Monarch.hpp"
#include "Vec2.hpp"
#include "Tile.hpp"

#include "events/Keyboard.hpp"
#include "components/World.hpp"

class Player : public Monarch::Component {
public:
    Player(World& world, Vec2 startPos);
    virtual ~Player(){}

    virtual void init() override;

    virtual void update() override;
    virtual void draw() override;
    virtual bool onEvent(Monarch::Event& e) override;

private:
    bool onKeyPressed(Monarch::KeyPressedEvent& e);
    bool move(Vec2 worldPos);
    World& m_world;

    Vec2 m_wPos;
};