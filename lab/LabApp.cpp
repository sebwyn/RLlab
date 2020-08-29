#include "LabApp.hpp"

#include "LabWorldLayer.hpp"

int LabApp::initWidth = 800;
int LabApp::initHeight = 600;

LabApp::LabApp()
 : Monarch::Application("RLLab", LabApp::initWidth, LabApp::initHeight)
{
    window->enableCursor();
    layers.pushLayer<LabWorldLayer>(LabApp::initWidth, LabApp::initHeight);
}
