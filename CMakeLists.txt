cmake_minimum_required(VERSION 3.10)

project(multi_agent LANGUAGES CXX)

# Set C++ compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra")

#add executables
add_executable(multi_agent
    src/example.cpp
    src/headers/Agent.h
    src/headers/SectionPool.h 
    src/headers/System.h
)
