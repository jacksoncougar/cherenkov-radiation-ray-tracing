//
// Created by root on 9/3/20.
//

#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include <array>
#include <thread>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"
#include "glsl.hpp"

int main(int argc, char *argv[]) {
    assert(argc == 3);
    program program(argv[1], argv[2]);
    exit(0);
}