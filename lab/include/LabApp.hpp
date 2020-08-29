#pragma once

#include "Monarch.hpp"

class LabApp : public Monarch::Application {
public:
    LabApp();

    virtual ~LabApp() override {}

    virtual void update() override {}
private:
    static int initWidth, initHeight;
};
