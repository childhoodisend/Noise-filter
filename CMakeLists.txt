cmake_minimum_required(VERSION 3.17)
project(noise-filter)

set(CMAKE_CXX_STANDARD 14)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message(" ")
    message("CMAKE IN DEBUG MODE")
    message(" ")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    #    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
    message(" ")
    message("CMAKE IN RELEASE MODE")
    message(" ")
endif ()

add_executable(noise-filter main.cpp)