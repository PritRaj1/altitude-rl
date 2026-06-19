cmake_minimum_required(VERSION 3.15)
project(mars-lander LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

add_executable(main ${SOURCES} ${HEADERS})
target_include_directories(main PRIVATE include)
install(TARGETS main DESTINATION bin)
