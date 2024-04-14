cmake_minimum_required(VERSION 3.14)
project(babymd5 CXX)

set(CMAKE_CXX_COMPILER "clang++")

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	add_compile_options(-g -Wall -Wextra -Wpedantic)
	add_compile_definitions(DEBUG)
	add_link_options(-fsanitize=address,undefined)
else()
	add_compile_options(-O3)
endif()


add_executable(babymd5
	src/md5.cpp
)