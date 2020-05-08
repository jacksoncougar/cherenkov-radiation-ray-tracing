//
// Created by root on 9/3/20.
//
#include <conio.h>
#include <stdio.h>

#include <glad/glad.h>
// glad must come before glfw
#include "GLFW/glfw3.h"

#include <array>
#include <cxxopts.hpp>
#include <thread>

#include "program.hpp"

void reload_render_params() {
  std::fstream fh;
  fh.open(".settings", std::fstream::in | std::fstream::binary);
  if (fh.is_open()) {
    fh.read((char *)&params, sizeof(params));
  }
  params.batch = false;
}

namespace std {
std::string to_string(const Point3D &point) {
  return std::to_string(point.x) + " " + std::to_string(point.y) + " " +
         std::to_string(point.z);
}
} // namespace std

namespace std {
std::string to_string(const RGBColor &color) {
  return std::to_string(color.r) + " " + std::to_string(color.g) + " " +
         std::to_string(color.b);
}
} // namespace std

namespace std {
std::string to_string(const DepthRenderingParameters &param) {
  std::ostringstream ss;
  ss << param.gather_absorption << " " << param.gather_scattering << " "
     << param.irradiance_samples << " " << param.primary_samples << " "
     << param.secondary_samples;
  return ss.str();
}
} // namespace std

int main(int argc, char *argv[]) {
  cxxopts::Options options("Tracer [EMISSIVE_MESH_FILENAME] [CHAMBER_MESH_FILENAME]",
                           "C++ Volumetric Cherenkov Radiation Ray Marching");

  // reload_render_params();

  options.add_options()("batch", "Closes the program when the render finishes")(
      "p,camera-position", "The position of the camera in the scene",
      cxxopts::value<Point3D>()->default_value(
          std::to_string(params.camera_position)))(
      "t,target", "The target to point the camera towards",
      cxxopts::value<Point3D>()->default_value(std::to_string(params.target)))(
      "c,colour", "The colour of the glow",
      cxxopts::value<RGBColor>()->default_value(std::to_string(
          params.glow_colour)))("gather-surface",
                                "Enables surface radiosity gathering",
                                cxxopts::value<bool>()->default_value(
                                    std::to_string(params.gather_surface)))

      ("e,extinction",
       "The amount of light that is extinguished per unit of distance",
       cxxopts::value<float>()->default_value(
           std::to_string(params.extinction)))(
          "f,falloff",
          "Controls the distance at which light is emitted in the volume",
          cxxopts::value<float>()->default_value(
              std::to_string(params.falloff)))(
          "a,absorption",
          "The amount of light that is emitted per unit of distance",
          cxxopts::value<float>()->default_value(std::to_string(params.a)))(
          "s,scattering",
          "The amount of light that in-scatters per unit of distance",
          cxxopts::value<float>()->default_value(std::to_string(params.s)))(
          "i,intensity",
          "The brightness of the light that is emitted in the volume",
          cxxopts::value<float>()->default_value(
              std::to_string(params.intensity)))

          ("p0", "Depth 0 ray marching parameters",
           cxxopts::value<DepthRenderingParameters>()->default_value(
               std::to_string(params.parameters[0])))(
              "p1", "Depth 1 ray marching parameters",
              cxxopts::value<DepthRenderingParameters>()->default_value(
                  std::to_string(params.parameters[1])))(
              "p2", "Depth 2 ray marching parameters",
              cxxopts::value<DepthRenderingParameters>()->default_value(
                  std::to_string(params.parameters[2])))("h,help",
                                                         "Print usage");

  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    exit(0);
  }

  params.batch =
      result.count("batch") ? result["batch"].as<bool>() : params.batch;

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