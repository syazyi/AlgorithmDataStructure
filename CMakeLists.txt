cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(myLib)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_VS_PLATFORM_TOOLSET)
add_subdirectory(src)
