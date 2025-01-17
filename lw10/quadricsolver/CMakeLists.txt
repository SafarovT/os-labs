cmake_minimum_required(VERSION 3.20)
project(quadricsolver)

set(CMAKE_CXX_STANDARD 20)

add_definitions(-COMPILING_DLL)

add_library(quadricsolver SHARED quadricsolver.cpp)

target_include_directories(quadricsolver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})