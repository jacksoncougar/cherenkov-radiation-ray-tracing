cmake_minimum_required(VERSION 3.10)
project(wxraytracer LANGUAGES CXX)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE 3rd-party/stb)

add_library(tinyply STATIC 3rd-party/tinyply/tinyply.cpp)
target_include_directories(tinyply PUBLIC 3rd-party/tinyply)

add_subdirectory(raytracer)
