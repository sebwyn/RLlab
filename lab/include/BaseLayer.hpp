#pragma once

#include "Monarch.hpp"

#include "components/Transform.hpp"
#include "components/3d/Camera.hpp"

class BaseLayer : public Monarch::Layer {
private:
    Monarch::Manager m_manager; 

    Monarch::Entity& m_camera;
public:
    BaseLayer(int initWidth, int initHeight)
     : m_camera(m_manager.addEntity())
    {
        Monarch::Transform& transform = m_camera.addComponent<Monarch::Transform>();
        Monarch::Camera& cam = 
            m_camera.addComponent<Monarch::Camera>(initWidth, initHeight);

        //make some basic camera settings, because this this is a 2d application
        //so we can assume some things
        cam.ortho();

        transform.translate(0, 0, 100);
        transform.rotate(0, 3.141592, 0);
    }

    virtual void init() override {}

    virtual void update() override
    {
        m_manager.update();
        m_manager.cleanUp();

        Monarch::Camera& cam = m_camera.getComponent<Monarch::Camera>();
        Monarch::Renderer::setMatrices(cam.getProjectionMatrix(), cam.getViewMatrix());

        Monarch::Renderer::beginBatch();
        m_manager.draw();
        Monarch::Renderer::endBatch();

        Monarch::Renderer::flush();
    }
    virtual bool onEvent(Monarch::Event& e) override { return m_manager.onEvent(e); }

    Monarch::Entity& addEntity() { return m_manager.addEntity(); }
    Monarch::Entity& getCamera() { return m_camera; }
};
