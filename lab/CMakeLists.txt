cmake_minimum_required(VERSION 3.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab BasicDungeonGenerator.cpp Camera.cpp DenseNN.cpp Entity.cpp LevelGenerator.cpp Player.cpp Tile.cpp Vec2.cpp main.cpp)
target_link_libraries(lab Eigen curses)
