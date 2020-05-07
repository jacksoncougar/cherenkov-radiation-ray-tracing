cmake_minimum_required(VERSION 3.10)
project(wxraytracer LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_RELEASE "/Ox")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_REQUIRED_LIBRARIES legacy_stdio_definitions.lib)

if(!MSVC)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")
endif()
find_library(legacy legacy_stdio_definitions )
message(${legacy})

add_library(glad 3rd-party/glad/src/glad.c)
target_include_directories(glad PUBLIC 3rd-party/glad/include)
target_link_libraries(glad ${CMAKE_DL_LIBS})
target_link_libraries(glad ${legacy})




set(NANOGUI_BUILD_EXAMPLE OFF CACHE BOOL " " FORCE)
set(NANOGUI_BUILD_PYTHON  OFF CACHE BOOL " " FORCE)
set(NANOGUI_INSTALL       OFF CACHE BOOL " " FORCE)
add_subdirectory(nanogui)

add_library(opt INTERFACE)
target_include_directories(opt INTERFACE 3rd-party/cxxopts/include)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE 3rd-party/stb)


add_subdirectory(raytracer)
 