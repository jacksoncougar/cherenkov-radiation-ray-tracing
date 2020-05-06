//
// Created by Jackson Cougar Wiebe on 3/15/2020.
//

#include "MarchingRayTracer.hpp"
#include <ShadeRec.h>
#include <cmath>
#include <algorithm>
#include "World.h"
#include "Material.h"
#include <functional>
#include <random>
#include <iomanip>

RGBColor MarchingRayTracer::trace_ray(const Ray& ray) const {
	return Tracer::trace_ray(ray);
}


std::ostream& operator<<(std::ostream& out, const RayMarchingParameters& params) {

	out << "# Parameters\n";
	out << "absorbance=" << params.a << '\n';
	out << "scattering=" << params.s << '\n';
	out << "glow_colour=" << params.glow_colour << '\n';
	out << "extinction=" << params.extinction << '\n';
	out << "intensity=" << params.intensity << '\n';

	out << "# Samples\n";
	out << "steps:0=" << params.parameters[0].primary_samples << '\n';
	out << "in-scattering:0=" << params.parameters[0].secondary_samples << '\n';
	out << "irradiance:0=" << params.parameters[0].in_scattering_samples << '\n';
	out << "steps:1=" << params.parameters[1].primary_samples << '\n';
	out << "in-scattering:1=" << params.parameters[1].secondary_samples << '\n';
	out << "irradiance:1=" << params.parameters[0].in_scattering_samples << '\n';

	out << "# Gathers\n";
	out << "surface:0=" << params.gather_surface << '\n';
	out << "absorption:0=" << params.parameters[0].gather_absorption << '\n';
	out << "scattering:0=" << params.parameters[0].gather_scattering << '\n';
	out << "irradiance:0=" << params.parameters[0].in_scattering_samples << '\n';
	out << "absorption:1=" << params.parameters[1].gather_absorption << '\n';
	out << "scattering:1=" << params.parameters[1].gather_scattering << '\n';
	out << "irradiance:0=" << params.parameters[0].in_scattering_samples << '\n';

	out << "# Args\n";
	out << "-a " << params.a << " ";
	out << "-s " << params.s << " ";
	out << "-e " << params.extinction << " ";
	out << "-i " << params.intensity << " "; 
	out << "-f " << params.falloff << " ";
	out << "--gather-surface=" << params.gather_surface << " ";
	out << "--camera-position=\"" << params.camera_position << "\" ";
	out << "--target=\"" << params.target << "\" ";
	out << "--colour=\"" << params.glow_colour << "\" ";
	out << "--p0=\"" << params.parameters[0] << "\" ";
	out << "--p1=\"" << params.parameters[1] << "\" ";
	out << "--p2=\"" << params.parameters[2] << "\" ";

	return out;
}

RayMarchingParameters params = RayMarchingParameters();


LevelParameters getParameters(int depth) {
	switch (depth)
	{
	case 0: return params.parameters[0];
	case 1: return params.parameters[1];
	case 2: return params.parameters[2];
	default: return { 0,0 };
	}
}

#define dot *

// absorption coefficient at point p0
auto mu_a = [](float t) {
	return t * params.a;
};

// absorption coefficient at point t
auto mu_s = [](float t) {
	return t * params.s;
};

// extinction coefficient at point t
auto mu_t = [&](float t) {
	return mu_s(t) + mu_s(t);
};

// phase function
constexpr auto f_p = [&](Vector3D /*w*/, Vector3D /*w_bar*/) { return 1.0 / (4.0 * PI); };

auto random_variable_raw = []() {
	static std::random_device rd;
	static std::minstd_rand e2(rd());
	static std::uniform_real_distribution<> dist(0, 1);
	return dist(e2);
};


auto random_variable = []() {
	return random_variable_raw();
};

float calculate_zeroth() {
	auto result = 1 / mu_t(280);
	return result;
}

auto zeroth = calculate_zeroth();

auto sample_t = [&]() {
	auto xi = random_variable();
	auto result = -std::log(1 - xi) / zeroth;
	return result;
};

auto S2_sample_raw = [&]() {
	float u = random_variable();
	float v = random_variable();
	float theta = 2 * PI * u;
	float phi = std::acos(2 * v - 1);
	float x = std::sin(theta) * std::cos(phi);
	float y = std::sin(theta) * std::sin(phi);
	float z = std::cos(theta);
	return Vector3D(x, y, z);
};

auto S2_sample = [&]() {
	thread_local std::array<Vector3D, 10000> samples; 
	thread_local int i = 0;
	thread_local int fill = 0;


	if (fill < samples.size()) {
		samples[fill] = S2_sample_raw();
		return samples[fill++];
	}
	else return samples[(size_t)(random_variable() * fill) % fill];
};

// radiance emitted from a volume at point x in direction w
auto Le = [&](Point3D x, Vector3D w, World* world) -> RGBColor {

	RGBColor result{ 0 };
	ShadeRec sr(*world); 
	sr.hit_point = x;

	for (auto&& emission_source : world->lights) {
		result += emission_source->L(sr);
	}

	return result;
};

auto tau = [&](float t) {
	return t * params.extinction;
};

//transmittance
auto T_r = [&](float t) {
	return  std::exp(-tau(t));
};
 
auto p = [&](float t) {
	auto result = zeroth * std::exp(-zeroth * t);
	return result;
};

// incident radiance at point in direction w
RGBColor MarchingRayTracer::Li(Point3D x, Vector3D w, int depth, World* world) const {
	return trace_ray({ x,w }, depth + 1);
};



auto _if = [&](bool condition, auto result, auto default) {
	if (condition) return result;
	else return default;
};

// in-scattering radiance
RGBColor MarchingRayTracer::Ls(Point3D x, Vector3D w, int depth, World * world) const {

	RGBColor result{ 0 };

	auto number_of_samples = getParameters(depth).secondary_samples;
	for (int i = 0; i < number_of_samples; ++i)
	{
		Vector3D w_bar = S2_sample();
		result += f_p(w, w_bar) * Li(x, w_bar, depth, world);
	}
	return result / std::max(1, number_of_samples);
};



// in-scattering radiance at surface
RGBColor MarchingRayTracer::Ls2(Point3D x, Vector3D n, int depth, World * world) const {

	RGBColor result{ 0 };

	auto gs = getParameters(depth).gather_scattering;
	if (!gs) return result;

	auto number_of_samples = getParameters(depth).in_scattering_samples;
	for (int i = 0; i < number_of_samples; ++i)
	{
		Vector3D w_bar = S2_sample();
		result += f_p(n, w_bar) * std::max(0.0, n dot w_bar) * trace_ray(Ray{ x, w_bar }, depth + 1);

	}
	return result / std::max(1, number_of_samples);
};


RGBColor MarchingRayTracer::Sample(Point3D x, Point3D y, float t, Vector3D w, World * world, int depth) const {

	auto ga = getParameters(depth).gather_absorption;
	auto gs = getParameters(depth).gather_scattering;
	return T_r(t) / p(t) * (
		_if(ga, mu_a(t) * Le(y, w, world), RGBColor{ 0 }) +
		_if(gs, mu_s(t) * Ls(y, w, depth, world), RGBColor{ 0 })
		);
};

RGBColor MarchingRayTracer::monte_carlo(Point3D x, Vector3D w, int depth, World * world, float max_t) const
{
	RGBColor result = { 0 };
	auto number_of_samples = getParameters(depth).primary_samples;
	auto steps = number_of_samples;
	auto step_t = max_t / steps;

	for (int step = 0; step < steps; ++step) {
		auto t = step_t * step;
		auto y = x + w * t;
		result += Sample(x, y, t, w, world, depth);
	}

	return result / std::max<size_t>(1, steps);
};

RGBColor MarchingRayTracer::trace_ray(const Ray ray, const int depth) const {

	// plan: cast a ray and see where it hits the scene...
	// then subdivide that distance by some value and sample backwards along the ray to accumulate
	// light in the medium.
	// the amount of light at a given point is a function of distance from the emitter

	ShadeRec sr(world_ptr->hit_objects(ray));

	auto [x, w] = ray;

	auto tone_map_john_hable = [](const RGBColor& colour) -> RGBColor {

		float A = 0.15;
		float B = 0.50;
		float C = 0.10;
		float D = 0.20;
		float E = 0.02;
		float F = 0.30;

		return ((colour * (A * colour + C * B) + D * E) / (colour * (A * colour + B) + D * F)) - E / F;
	};

	auto tone_map2 = [&](const RGBColor& colour) -> RGBColor {
		return tone_map_john_hable(colour) / tone_map_john_hable(RGBColor(1.5, 1.5, 1.5));
	};
	auto tone_map3 = [&](const RGBColor& colour) -> RGBColor {
		return colour / (colour + RGBColor(1));
	};
	 
	auto tone_map = [](const RGBColor& colour) -> RGBColor {
		auto [r, g, b] = colour; 
		float L = 0.2126 * r + 0.7152 * g + 0.0722 * b;
		auto nl = L / (L + 1);
		auto scale = nl / L; 
		r *= scale;
		g *= scale;
		b *= scale; 

		return { r,g,b };
	};

	if (sr.hit_an_object) {

		auto surface_color = params.gather_surface && sr.material_ptr ? T_r(sr.t) * sr.material_ptr->shade(sr) : 0;
		auto irradiance = T_r(sr.t) / p(sr.t) * Ls2(sr.hit_point, sr.normal, depth, world_ptr);
		auto transmission = monte_carlo(x, w, depth, world_ptr, sr.t);
		auto radiance = surface_color + irradiance + transmission;
		if (depth == 0)
			return tone_map3(radiance);
		else
			return radiance;

	}
	else {

		auto transmission = monte_carlo(x, w, depth, world_ptr, 140.0f);
		if (depth == 0)
			return  tone_map3(transmission);
		else
			return transmission;
		 
	}
}

MarchingRayTracer::MarchingRayTracer(
	World * pWorld
) : Tracer(pWorld) {}
