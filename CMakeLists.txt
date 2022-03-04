cmake_minimum_required(VERSION 3.0)
project(pathfinder)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(pathfinder
	main.cpp
	lodepng.cpp
	lodepng.h
	picture.cpp
	picture.h
	pathfinder.cpp
	pathfinder.h)

