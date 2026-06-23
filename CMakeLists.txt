cmake_minimum_required(VERSION 3.15)
project(mars-lander LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
    src/agent.cpp
    src/env.cpp
    src/pid.cpp
    src/train.cpp
    src/utils.cpp
)

add_executable(main src/main.cpp ${SOURCES})
target_include_directories(main PRIVATE include)

install(TARGETS main DESTINATION bin)
