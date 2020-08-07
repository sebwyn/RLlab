#include "Camera.hpp"

#include "RoguelikeGame.hpp"

void Camera::update(){
    int width, height;
    getmaxyx(stdscr, height, width);
    int r = 0;
    for(int wr = m_y; wr < m_y + height; wr++){
        std::string rowText;
        for(int wc = m_x; wc < m_x + width; wc++){
            //check to make sure the current tile is in the world bounds otherwise just draw a blank tile
            if(-1 < wr && wr < m_game->getHeight() && -1 < wc && wc < m_game->getWidth()) rowText += m_game->getWorld()[wr][wc].sym; 
            else rowText += ' ';
        }
        mvprintw(r, 0, rowText.c_str());
        r++;
    }
}

void Camera::handleInput(int key){
    switch(key){
        case 'h': m_x--; break;
        case 'j': m_y++; break;
        case 'k': m_y--; break;
        case 'l': m_x++; break;
    }
}

void Camera::initRender(){
    initscr();
    
    timeout(0);
    cbreak();
    noecho();
    keypad(stdscr, true);
}

void Camera::destroyRender(){
    nocbreak();
    echo();
    endwin();
}
