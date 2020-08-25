cmake_minimum_required(VERSION 3.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
     cmake_policy(VERSION 3.15)
endif()

project(MyProject VERSION 1.0
                  DESCRIPTION "A reinforcement learning project built from scratch"
                  LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)

add_executable(lab main.cpp Entity.cpp Camera.cpp BasicDungeonGenerator.cpp Tile.cpp)
target_link_libraries(lab curses)
#set_property(TARGET lab PROPERTY CMAKE_CXX_FLAGS_DEBUG "-g")
