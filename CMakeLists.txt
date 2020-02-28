cmake_minimum_required(VERSION 3.10)
project(wxraytracer LANGUAGES CXX)

add_library(stb 3rd-party/stb/stb_image.h)

add_subdirectory(raytracer)