#pragma once

#include <string>
#include <iostream>
#include <vector>

#include <curses.h> 

#include "Entity.hpp"

class RoguelikeGame {
public:
   RoguelikeGame(bool liveSession = true) : m_isLive(liveSession) {
        if(m_isLive) initRender();
    }
     
    ~RoguelikeGame(){
        if(m_isLive) destroyRender();
    } 
     
    Entity& addEntity(){
        m_entities.emplace_back();
        return m_entities.back(); 
    }
    
    void start(){
        while(m_running){
            update();
            if(m_isLive){ 
                handleInput();
                render();
            }
        }
    }
    
private:
    bool m_isLive, m_running = true;
    std::vector<Entity> m_entities;
    
    void initRender(){
        initscr();
        
        cbreak();
        noecho();
        keypad(stdscr, true);
    }
    
    void destroyRender(){
        endwin();
        exit(0);
    }
    
    void update(){
        //global physics update as the game handles the world
    
        //entity update
        /*for(Entity ent : entities){
            ent.update();
        }*/
    }
    
    void handleInput(){
        switch(getch()){
            case 'q': 
                m_running = false;
                break;
        }
    }

    void render(){
        //render level
    
        //render entities
    }
};
