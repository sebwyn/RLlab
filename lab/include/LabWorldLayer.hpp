#pragma once

#include "Monarch.hpp"
#include "components/3d/Camera.hpp"

class LabWorldLayer : public Monarch::Layer {
public: 
    LabWorldLayer(int winWidth, int winHeight);

    virtual void init() override;
    virtual void update() override;

    virtual bool onEvent(Monarch::Event& e) override;
private:
    int winWidth, winHeight;

    Monarch::Manager m_entities;

    Monarch::Entity& m_cameraEntity = m_entities.addEntity();  
};
