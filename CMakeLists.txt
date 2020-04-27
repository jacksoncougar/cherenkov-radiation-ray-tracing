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

add_subdirectory(lib/nanogui)
include_directories(lib/nanogui/include)
include_directories(${NANOGUI_EXTRA_INCS})
add_definitions(${NANOGUI_EXTRA_DEFS})

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE 3rd-party/stb)

add_library(json INTERFACE)
target_include_directories(json INTERFACE 3rd-party/nlohmann/include)


add_subdirectory(raytracer)
 