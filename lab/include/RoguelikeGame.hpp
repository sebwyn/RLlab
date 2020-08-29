#pragma once

#include <string>
#include <iostream>
#include <vector>

#include <curses.h> 

#include "Entity.hpp"
#include "Tile.hpp"

class RoguelikeGame {
public:
    RoguelikeGame(){}
    
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
    inline int getWidth(){ return world.back().size(); }
    inline int getHeight(){ return world.size(); }
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
