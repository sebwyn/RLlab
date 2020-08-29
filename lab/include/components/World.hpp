#pragma once

#include "Monarch.hpp"

class World : public Monarch::Component {
public:
    virtual void init() override {}
    virtual void update() override {}
    virtual void draw() override 
    {
        Monarch::Renderer::QuadData quad;
        quad.color = glm::vec4(1, 0, 1, 1);
        quad.size = glm::vec2(100, 100);
        Monarch::Renderer::drawQuad(quad);
    }

    virtual bool onEvent(Monarch::Event& e) override
    {
        return false;
    }
};
