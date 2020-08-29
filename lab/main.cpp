#include <iostream>

#include "Monarch.hpp"
#include "EntryPoint.hpp"

#include "LabApp.hpp"

Monarch::Application* Monarch::createApplication()
{
    return new LabApp();
}


