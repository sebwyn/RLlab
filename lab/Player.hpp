#pragma once

#include "RoguelikeGame.hpp"
#include "Component.hpp"
#include "Vec2.hpp"

class Player : public Component {
public:
    Player(RoguelikeGame* game, Vec2 startPos);
    virtual ~Player(){}

    virtual void update() override;
    virtual void handleInput(int key) override;

    bool move(Vec2 newPos);
private:
    Tile tileAtPos;
    
    RoguelikeGame* m_game;
    Vec2 m_pos;
};