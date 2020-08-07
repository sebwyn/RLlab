#pragma once

#include <string>
#include <iostream>
#include <vector>

#include <curses.h> 

#include "Entity.hpp"
#include "Tile.hpp"

class RoguelikeGame {
public:
    RoguelikeGame(int width, int height) : m_width(width), m_height(height) {}
    
    ~RoguelikeGame(){
        for(Entity& ent : m_entities){
            ent.destroy();
        }
    }

    Entity& addEntity(){
        m_entities.emplace_back();
        return m_entities.back(); 
    }

    void start(){
        while(m_running){
            update();
            handleInput();
        }
    }

    inline std::vector<std::vector<Tile>>& getWorld(){ return world; }
    inline int getWidth() { return m_width; }
    inline int getHeight() { return m_height; }

private:
    bool m_running = true;
    int m_width, m_height;

    std::vector<Entity> m_entities;
    std::vector<std::vector<Tile>> world;

    void update(){
        //entity update
        for(Entity& ent : m_entities){
            ent.update();
        }
    }

    void handleInput(){
        int key = getch();
        switch(key){
            case 'q': 
                m_running = false;
                break;
            default:
                for(Entity& ent : m_entities){
                    ent.handleInput(key);
                }
        }
    }
};
