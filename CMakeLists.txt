cmake_minimum_required(VERSION 3.6)
project(CountingFP)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(CountingFP ${SOURCE_FILES})

TARGET_LINK_LIBRARIES( CountingFP pthread)