#include "components/Player.hpp"

#include "components/Transform.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>


Player::Player(World& world, Vec2 startPos) 
 : m_world(world), 
   m_wPos(startPos)
{
}

void Player::init() 
{
    m_entity->getComponent<Monarch::Transform>().setPosition(m_wPos.x * m_world.getTileSize(), m_wPos.y * m_world.getTileSize(), 0);
}

void Player::update(){
}

void Player::draw()
{
    Monarch::Renderer::QuadData quad;
    quad.position = m_entity->getComponent<Monarch::Transform>().getPosition();
    quad.size = glm::vec2(m_world.getTileSize(), m_world.getTileSize());
    quad.color = TileManager::player.color;  
    Monarch::Renderer::drawQuad(quad);
}

bool Player::onEvent(Monarch::Event& e)
{
    e.dispatch<Monarch::KeyPressedEvent>(std::bind(&Player::onKeyPressed, this, std::placeholders::_1));
    return false;
}

bool Player::onKeyPressed(Monarch::KeyPressedEvent& e)
{
    switch(e.getKey()){
        case GLFW_KEY_W:
            move(m_wPos + north);
            break;
        case GLFW_KEY_A:
            move(m_wPos + west);
            break;
        case GLFW_KEY_S:
            move(m_wPos + south);
            break;
        case GLFW_KEY_D:
            move(m_wPos + east);
            break;
    }
    return false;
}

bool Player::move(Vec2 worldPos)
{
    Monarch::Transform& t = m_entity->getComponent<Monarch::Transform>();
    if (m_world.getTile(worldPos) && *(m_world.getTile(worldPos)) == TileManager::wall){
        return false;
    } else {
        m_wPos = worldPos;
        t.setPosition(m_wPos.x * m_world.getTileSize(), m_wPos.y * m_world.getTileSize(), 0);
        return true;
    }
}