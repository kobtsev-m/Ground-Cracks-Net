cmake_minimum_required(VERSION 3.17)
project(dataset_generator)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(lib/fft)
include_directories(lib/fft/include)

add_subdirectory(lib/json)
include_directories(lib/json/include)

file(GLOB SOURCES src/*.cpp src/*.h)
add_executable(${PROJECT_NAME} ${SOURCES})
