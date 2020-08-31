#pragma once

#include "Monarch.hpp"

#include "BaseLayer.hpp"

class LabApp : public Monarch::Application {
public:
    LabApp();

    virtual ~LabApp() override {}

    virtual void update() override {}
private:
    static int initWidth, initHeight;

    std::shared_ptr<BaseLayer> m_ui, m_world;
};
