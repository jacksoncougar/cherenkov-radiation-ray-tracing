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

void reload_render_params() {
	std::fstream fh;
	fh.open(".settings", std::fstream::in | std::fstream::binary);
	if (fh.is_open())
	{
		fh.read((char*)&params, sizeof(params));
	}
	params.batch = false;
}

namespace std {
	std::string to_string(const Point3D& point) {
		return std::to_string(point.x) + " " + std::to_string(point.y) + " " + std::to_string(point.z); 
	}
}

namespace std {
	std::string to_string(const RGBColor& color) {
		return std::to_string(color.r) + " " + std::to_string(color.g) + " " + std::to_string(color.b); 
	}
} 

namespace std {
	std::string to_string(const DepthRenderingParameters& param) {
		std::ostringstream ss;
		ss << param.gather_absorption << " " << param.gather_scattering << " " << param.irradiance_samples << " "
			<< param.primary_samples << " " << param.secondary_samples;
		return ss.str();
	}
}


int main(int argc, char* argv[]) {
	cxxopts::Options options("C++ Volumetric Ray Tracer", "Traces rays");

	//reload_render_params();


	options.add_options()
		("batch", "")
		("p,camera-position", "", cxxopts::value<Point3D>()->default_value(std::to_string(params.camera_position)))
		("t,target", "", cxxopts::value<Point3D>()->default_value(std::to_string(params.target)))
		("c,colour", "", cxxopts::value<RGBColor>()->default_value(std::to_string(params.glow_colour)))
		("gather-surface", "", cxxopts::value<bool>()->default_value(std::to_string(params.gather_surface)))

		("e,extinction", "", cxxopts::value<float>()->default_value(std::to_string(params.extinction)))
		("f,falloff", "", cxxopts::value<float>()->default_value(std::to_string(params.falloff)))
		("a,absorption", "", cxxopts::value<float>()->default_value(std::to_string(params.a)))
		("s,scattering", "", cxxopts::value<float>()->default_value(std::to_string(params.s)))
		("i,intensity", "", cxxopts::value<float>()->default_value(std::to_string(params.intensity)))

		("p0", "", cxxopts::value<DepthRenderingParameters>()->default_value(std::to_string(params.parameters[0])))
		("p1", "", cxxopts::value<DepthRenderingParameters>()->default_value(std::to_string(params.parameters[1])))
		("p2", "", cxxopts::value<DepthRenderingParameters>()->default_value(std::to_string(params.parameters[2])))
		;

	auto result = options.parse(argc, argv);

	params.batch = result.count("batch") ? result["batch"].as<bool>() : params.batch;

	params.gather_surface = result["gather-surface"].as<bool>();
	params.camera_position = result["camera-position"].as<Point3D>();
	params.target = result["target"].as<Point3D>();
	params.glow_colour = result["colour"].as<RGBColor>();

	params.extinction = result["extinction"].as<float>();
	params.a = result["absorption"].as<float>();
	params.falloff = result["falloff"].as<float>();
	params.s = result["scattering"].as<float>();
	params.intensity = result["intensity"].as<float>();

	params.parameters[0] = result["p0"].as<DepthRenderingParameters>();
	params.parameters[1] = result["p1"].as<DepthRenderingParameters>();
	params.parameters[2] = result["p2"].as<DepthRenderingParameters>();

	program program(argv[1], argv[2]);

	exit(0);
}