//
// Created by root on 9/3/20.
//
#include <stdio.h>
#include <conio.h>


#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include <array>
#include <thread>

#include <cxxopts.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "glsl.hpp"

int main(int argc, char* argv[]) {
	cxxopts::Options options("MyProgram", "One line description of MyProgram");
	options.add_options()
		("d,debug", "Enable debugging") // a bool parameter
		("i,integer", "Int param", cxxopts::value<int>())
		("f,file", "File name", cxxopts::value<std::string>())
		("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
		;

	assert(argc == 3);
	program program(argv[1], argv[2]);

	exit(0);
}