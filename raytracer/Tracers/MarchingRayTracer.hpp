//
// Created by Jackson Cougar Wiebe on 3/15/2020.
//

#ifndef WXRAYTRACER_MARCHINGRAYTRACER_HPP
#define WXRAYTRACER_MARCHINGRAYTRACER_HPP

#include <array>
#include <iostream>
#include <vector>

#include "Tracer.h"
#include "RGBColor.h"
#include "Ray.h"

class MarchingRayTracer : public Tracer {
public:

	MarchingRayTracer(World* pWorld);
	[[nodiscard]] RGBColor trace_ray(const Ray& ray) const override;
	[[nodiscard]] RGBColor trace_ray(const Ray ray, const int depth) const override;

	MarchingRayTracer* clone() override {
		return new MarchingRayTracer(world_ptr);
	}

	RGBColor Li(Point3D x, Vector3D w, int depth, World* world)const;
	RGBColor Ls(Point3D x, Vector3D w, int depth, World* world)const;
	RGBColor Ls2(Point3D x, Vector3D w, int depth, World* world)const;
	RGBColor Sample(Point3D x, Point3D y, float t, Vector3D w, World* world, int depth)const;
	RGBColor monte_carlo(Point3D x, Vector3D w, int depth, World* world, float max_t = 100.0f)const;
};

struct LevelParameters
{
	int primary_samples;
	int secondary_samples;
	int in_scattering_samples = 0;
	bool gather_absorption = true;
	bool gather_scattering = true;
};


struct RayMarchingParameters
{
	bool batch = false;
	Point3D camera_position{};
	Point3D target{};

	bool gather_surface = true;
	std::array<Point3D,1> emitter_locations{
		Point3D{ 0, 0.2, 0 },
		//Point3D{-1.6, 0.2, 0},
		//Point3D{-3.2, 0.2, 0},
		//Point3D{ 0, 0.2, -1.74446 },
		//Point3D{-3.2, 0.2,-1.74446},
		//Point3D{ 0, 0.2, -3.47329 },
		//Point3D{-1.6, 0.2, -3.47329},
		//Point3D{-3.2, 0.2,-3.47329}
	};
	RGBColor glow_colour = RGBColor{ 2 / 255., 203 / 255., 213 / 255. };
	float intensity = 1;
	float a = 0.2;
	float s = 0.1;
	float extinction = 0.08;
	float falloff = 1;
	std::array<LevelParameters, 3> parameters = { LevelParameters{8, 8, 8}, LevelParameters{1, 1}, LevelParameters{1, 1} };

	RayMarchingParameters() {}
};

extern RayMarchingParameters params;

std::ostream& operator<<(std::ostream& out, const RGBColor& colour);

std::ostream& operator<<(std::ostream& out, const RayMarchingParameters& params);


inline std::istream& operator>>(std::istream& in, Point3D& point) {
	in >> point.x >> point.y >> point.z;
	return in;
}

inline std::istream& operator>>(std::istream& in, RGBColor& point) {
	in >> point.r >> point.g >> point.b;
	return in;
}

inline std::istream& operator>>(std::istream& in, LevelParameters& param) {
	in >> param.gather_absorption >> param.gather_scattering
		>> param.in_scattering_samples >> param.primary_samples >> param.secondary_samples;
	return in;
}


inline std::ostream& operator<<(std::ostream& out, const Point3D& point) {
	out << point.x << " " << point.y << " " << point.z;
	return out;

}

inline std::ostream& operator<<(std::ostream& out, const RGBColor& color) {
	out << color.r << " " << color.g << " " << color.b;
	return out;
}

inline std::ostream& operator<<(std::ostream& out, const LevelParameters& param) {
	out << param.gather_absorption << " " << param.gather_scattering << " " << param.in_scattering_samples << " "
		<< param.primary_samples << " " << param.secondary_samples;
	return out;
}


#endif //WXRAYTRACER_MARCHINGRAYTRACER_HPP
