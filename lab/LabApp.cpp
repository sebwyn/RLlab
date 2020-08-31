#include "LabApp.hpp"

#include "LabWorldLayer.hpp"

#include "components/2d/CamController2D.hpp"
#include "components/World.hpp"
#include "components/Player.hpp"

int LabApp::initWidth = 800;
int LabApp::initHeight = 600;

LabApp::LabApp()
 : Monarch::Application("RLLab", LabApp::initWidth, LabApp::initHeight)
{
    window->enableCursor();

    m_ui = std::static_pointer_cast<BaseLayer>(
        layers.pushLayer<BaseLayer>(LabApp::initWidth, LabApp::initHeight)
    );
    m_world = std::static_pointer_cast<BaseLayer>(
        layers.pushLayer<BaseLayer>(LabApp::initWidth, LabApp::initHeight)
    );

    m_world->getCamera().addComponent<Monarch::CamController2D>();

    Monarch::Entity& world = m_world->addEntity();
    world.addComponent<Monarch::Transform>();
    World& w = world.addComponent<World>(50, 50, 1337);

    Monarch::Entity& player = m_world->addEntity();
    player.addComponent<Monarch::Transform>();
    player.addComponent<Player>(w, w.getSpawnPoint());
}
