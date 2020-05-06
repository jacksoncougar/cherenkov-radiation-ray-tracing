//
// Created by Jackson Cougar Wiebe on 3/15/2020.
//

#ifndef WXRAYTRACER_MARCHINGRAYTRACER_HPP
#define WXRAYTRACER_MARCHINGRAYTRACER_HPP

#include <array>
#include <iostream>
#include <vector>

#include "RGBColor.h"
#include "Ray.h"
#include "Tracer.h"

class MarchingRayTracer : public Tracer {
public:
  MarchingRayTracer(World *pWorld);
  [[nodiscard]] RGBColor trace_ray(const Ray &ray) const override;
  [[nodiscard]] RGBColor trace_ray(const Ray ray,
                                   const int depth) const override;

  MarchingRayTracer *clone() override {
    return new MarchingRayTracer(world_ptr);
  }

  RGBColor Li(Point3D x, Vector3D w, int depth, World *world) const;
  RGBColor Ls(Point3D x, Vector3D w, int depth, World *world) const;
  RGBColor Ls2(Point3D x, Vector3D w, int depth, World *world) const;
  RGBColor Sample(Point3D x, Point3D y, float t, Vector3D w, World *world,
                  int depth) const;
  RGBColor monte_carlo(Point3D x, Vector3D w, int depth, World *world,
                       float max_t = 100.0f) const;
};

/// <summary>
/// Sampling and gather parameters for a given depth
/// </summary>
struct DepthRenderingParameters {
  int primary_samples = 0;
  int secondary_samples = 0;
  int irradiance_samples = 0;
  bool gather_absorption = true;
  bool gather_scattering = true;
};

struct RayMarchingParameters {
  // terminate program when render is finished
  bool batch = false;
  Point3D camera_position{};
  // world coordinates to point the camera
  Point3D target{};

  // adds light from surfaces
  bool gather_surface = true;
  std::array<Point3D, 1> emitter_locations{Point3D{0, 0.2, 0}};
  RGBColor glow_colour = RGBColor{2 / 255., 203 / 255., 213 / 255.};

  // how bright the light is
  float intensity = 100;
  // absorbance coefficient of the volume
  float a = 0.2;
  //scattering coefficient of the volume
  float s = 0.1;
  // extinction coefficient of the volume
  float extinction = 0.08;

  // my artist parameter--controls how far the light reaches within the volume
  float falloff = 2.5;

  std::array<DepthRenderingParameters, 3> parameters = {
      DepthRenderingParameters{8, 8, 8}, DepthRenderingParameters{2, 0},
      DepthRenderingParameters{0, 0}};

  RayMarchingParameters() {}
};

// Big ugly global
extern RayMarchingParameters params;

// IO functions

std::ostream &operator<<(std::ostream &out, const RGBColor &colour);

std::ostream &operator<<(std::ostream &out,
                         const RayMarchingParameters &params);

inline std::istream &operator>>(std::istream &in, Point3D &point) {
  in >> point.x >> point.y >> point.z;
  return in;
}

inline std::istream &operator>>(std::istream &in, RGBColor &point) {
  in >> point.r >> point.g >> point.b;
  return in;
}

inline std::istream &operator>>(std::istream &in,
                                DepthRenderingParameters &param) {
  in >> param.gather_absorption >> param.gather_scattering >>
      param.irradiance_samples >> param.primary_samples >>
      param.secondary_samples;
  return in;
}

inline std::ostream &operator<<(std::ostream &out, const Point3D &point) {
  out << point.x << " " << point.y << " " << point.z;
  return out;
}

inline std::ostream &operator<<(std::ostream &out, const RGBColor &color) {
  out << color.r << " " << color.g << " " << color.b;
  return out;
}

inline std::ostream &operator<<(std::ostream &out,
                                const DepthRenderingParameters &param) {
  out << param.gather_absorption << " " << param.gather_scattering << " "
      << param.irradiance_samples << " " << param.primary_samples << " "
      << param.secondary_samples;
  return out;
}

#endif // WXRAYTRACER_MARCHINGRAYTRACER_HPP
