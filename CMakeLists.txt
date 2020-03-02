cmake_minimum_required(VERSION 3.10)
project(wxraytracer LANGUAGES CXX)

add_library(stb 3rd-party/stb/stb_image.h raytracer/Materials/svattributebasedmapping.cpp raytracer/Materials/svattributebasedmapping.hpp raytracer/BRDFs/attribute.cpp raytracer/BRDFs/attribute.hpp)

add_subdirectory(raytracer)