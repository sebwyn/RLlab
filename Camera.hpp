#pragma once

#include <iostream>

#include "Component.hpp"

class RoguelikeGame;

class Camera : public Component {
public:
    Camera(RoguelikeGame* game) : m_game(game) {
        initRender();
    }
    
    virtual ~Camera() override {
        destroyRender();
    }

    virtual void update() override;
    virtual void handleInput(int key) override;
private:
    RoguelikeGame* m_game;
    int m_x = 0, m_y = 0;

    void initRender();
    void destroyRender();
};
