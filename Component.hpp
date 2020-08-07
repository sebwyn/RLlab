#pragma once

class Component {
public:

    virtual ~Component() {}

    virtual void update(){}
    virtual void handleInput(int key){}
private:
};
