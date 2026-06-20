cmake_minimum_required(VERSION 3.15)
project(mars-lander LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    src/env.cpp
    src/pid.cpp
    src/agent.cpp
    src/utils.cpp
)

add_executable(pid src/main_pid.cpp ${SOURCES})
target_include_directories(pid PRIVATE include)

add_executable(td src/main_td.cpp ${SOURCES})
target_include_directories(td PRIVATE include)

install(TARGETS pid td DESTINATION bin)
