cmake_minimum_required(VERSION 3.1)

add_library(Eigen INTERFACE)
target_include_directories(Eigen INTERFACE eigen)

add_subdirectory(Monarch)
