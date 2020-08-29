cmake_minimum_required(VERSION 3.1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB src_files *.cpp components/*.cpp)
add_executable(lab ${src_files})
target_link_libraries(lab Eigen curses MonarchRelease)
target_include_directories(lab PRIVATE include)
