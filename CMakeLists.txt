cmake_minimum_required(VERSION 3.15)
project(MyRaylibGame)

# This tells CMake to find the system-installed raylib library
find_package(raylib REQUIRED)

# This creates your game executable from main.cpp
add_executable(${PROJECT_NAME} main.cpp)

# This connects (links) raylib to your game
target_link_libraries(${PROJECT_NAME} raylib)
