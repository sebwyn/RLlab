#include "Camera.hpp"

#include "RoguelikeGame.hpp"

void Camera::update(){
    int width, height;
    getmaxyx(stdscr, height, width);
    int r = 0;
    for(int wr = m_y; wr < m_y + height; wr++){
        int c = 0;
        for(int wc = m_x; wc < m_x + width; wc++){
            //check to make sure the current tile is in the world bounds otherwise just draw a blank tile
            Tile t; 
            if(-1 < wr && wr < m_game->getHeight() && -1 < wc && wc < m_game->getWidth()) t = m_game->getWorld()[wr][wc]; 
            else t = TileManager::floor;
            attron(COLOR_PAIR(t.color));
            mvaddch(r, c, t.sym);
            c++;
        }
        r++;
    }
    refresh();
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

    TileManager::initColors();
}

void Camera::destroyRender(){
    nocbreak();
    echo();
    endwin();
}
