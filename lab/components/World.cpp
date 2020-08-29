#include "components/World.hpp"

#include "components/Transform.hpp"

World::World(int width, int height)
 : m_generator(BasicDungeonGenerator(m_data, width, height, 894725, 100, 4, 7))
{
}

void World::init()
{}
void World::update()
{}
void World::draw() 
{
    glm::vec3 position = m_entity->getComponent<Monarch::Transform>().getPosition();
    for(int x = 0; x < m_data.size(); x++){
        for(int y = 0; y < m_data[x].size(); y++){
            Monarch::Renderer::QuadData quad;
            quad.position = position + glm::vec3(m_tileSize)*glm::vec3(x, y, 0);
            quad.size = glm::vec2(m_tileSize, m_tileSize);
            quad.color = m_data[x][y].color;
            Monarch::Renderer::drawQuad(quad);
        }
    }
}
bool World::onEvent(Monarch::Event& e)
{
    return false;
}
