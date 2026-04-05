cmake_minimum_required(VERSION 3.16)
project(optioryx CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

include_directories(include src)

set(SOURCES
    src/main.cpp
    src/data_loader.cpp
    src/distance.cpp
    src/evaluator.cpp
    src/greedy/greedy.cpp
    src/simulated_annealing/sa.cpp
    src/genetic/ga.cpp
    src/seed_selection/seed_selection.cpp
)

add_executable(optioryx ${SOURCES})
