#include "LabWorldLayer.hpp"

#include "components/Transform.hpp"
#include "components/2d/CamController2D.hpp"

#include "components/World.hpp"

LabWorldLayer::LabWorldLayer(int winWidth, int winHeight)
 : winWidth(winWidth),
   winHeight(winHeight)
{}

void LabWorldLayer::init()
{
    Monarch::Entity& world = m_entities.addEntity();
    world.addComponent<Monarch::Transform>();
    world.addComponent<World>(50, 50);

    m_cameraEntity.addComponent<Monarch::Transform>();
    m_cameraEntity.addComponent<Monarch::CamController2D>();
    m_cameraEntity.addComponent<Monarch::Camera>(winWidth, winHeight);

    m_cameraEntity.getComponent<Monarch::Camera>().ortho();
}

void LabWorldLayer::update()
{
    m_entities.update();

    Monarch::Camera& cam = m_cameraEntity.getComponent<Monarch::Camera>();
    Monarch::Renderer::setMatrices(cam.getProjectionMatrix(), cam.getViewMatrix());

    Monarch::Renderer::beginBatch();
    m_entities.draw();
    Monarch::Renderer::endBatch();

    Monarch::Renderer::flush();
}

bool LabWorldLayer::onEvent(Monarch::Event& e)
{
    m_entities.onEvent(e);
    return false;
}

