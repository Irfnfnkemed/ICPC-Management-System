cmake_minimum_required(VERSION 3.22)
project(ICPC)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(.idea)

add_executable(ICPC
        .idea/.gitignore
        .idea/ICPC.iml
        .idea/modules.xml
        .idea/workspace.xml
        ICPC.cpp
        ICPC.h)
