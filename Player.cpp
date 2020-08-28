#include "Player.hpp"

Player::Player(RoguelikeGame* game, Vec2 startPos) : m_game(game) {
    m_pos = startPos; 
    tileAtPos = m_game->getWorld()[m_pos.r][m_pos.c];
    m_game->getWorld()[m_pos.r][m_pos.c] = TileManager::player;
}

void Player::update(){
}
void Player::handleInput(int key){
    switch(key){
        case 'w':
            move(m_pos + north);
            break;
        case 'a':
            move(m_pos + west);
            break;
        case 's':
            move(m_pos + south);
            break;
        case 'd':
            move(m_pos + east);
            break;
    }
}

bool Player::move(Vec2 newPos){
    if(m_game->getWorld()[newPos.r][newPos.c] == TileManager::wall){
        return false;
    } else {
        m_game->getWorld()[m_pos.r][m_pos.c] = tileAtPos;
        m_pos = newPos; 
        tileAtPos = m_game->getWorld()[m_pos.r][m_pos.c];
        m_game->getWorld()[m_pos.r][m_pos.c] = TileManager::player;
        return true;
    }
}