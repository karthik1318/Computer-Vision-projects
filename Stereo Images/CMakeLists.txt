cmake_minimum_required(VERSION 3.4)
project(akmehra-arkhande-kartsree-a4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_custom_target(akmehra-arkhande-kartsree-a4 ALL COMMAND mingw32-make WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})