#pragma once

#include "Component.hpp"

#include <vector>

class Entity {
public:

    void update();
    void handleInput(int key);
    
    void destroy(){
        for(Component* component : m_components){
            delete component;
        }
    }

    inline char getSym() const { return m_sym; }
    inline int getX() const { return m_x; }
    inline int getY() const { return m_y; }
     
    inline void setSym(char sym) { m_sym = sym; }
    inline void setX(int x) { m_x = x; }
    inline void setY(int y) { m_y = y; }

    void addComponent(Component* component);
    
private:
    std::vector<Component*> m_components;

    char m_sym;
    int m_x, m_y;
};
