cmake_minimum_required(VERSION 3.15)
project(mars-lander LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(main src/main.cpp)
install(TARGETS main RUNTIME DESTINATION bin)
