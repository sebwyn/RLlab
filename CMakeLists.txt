cmake_minimum_required(VERSION 3.1)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
     cmake_policy(VERSION 3.15)
endif()

project(MyProject VERSION 1.0
                  DESCRIPTION "A reinforcement learning project built from scratch"
                  LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(ext)
add_subdirectory(lab)
