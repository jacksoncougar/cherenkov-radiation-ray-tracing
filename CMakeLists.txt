cmake_minimum_required(VERSION 3.10)
project(wxraytracer LANGUAGES CXX C)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
add_library(glad STATIC 3rd-party/glad/src/glad.c)
target_include_directories(glad PUBLIC 3rd-party/glad/include)
target_link_libraries(glad ${CMAKE_DL_LIBS})

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE 3rd-party/stb)

add_library(json INTERFACE)
target_include_directories(json INTERFACE 3rd-party/nlohmann/include)

add_library(tinyply STATIC 3rd-party/tinyply/tinyply.cpp)
target_include_directories(tinyply PUBLIC 3rd-party/tinyply)

add_subdirectory(raytracer)
add_subdirectory(assets)
