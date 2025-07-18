cmake_minimum_required(VERSION 3.31)
project(untitled)

set(CMAKE_CXX_STANDARD 20)

add_executable(untitled main.cpp
        crosshair_remover.cpp
        crosshair_remover.h
        Line.cpp
        Line.h
        ellipse.cpp
        ellipse.h
        line.cpp)
