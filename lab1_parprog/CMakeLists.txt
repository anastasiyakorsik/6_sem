cmake_minimum_required(VERSION 3.19)

project(MPI_lab1)

find_package(MPI REQUIRED)

include_directories(lab1 ${MPI_INCLUDE_PATH})

add_executable(lab1 lab1.c)
target_link_libraries(lab1
        ${MPI_C_LIBRARIES})