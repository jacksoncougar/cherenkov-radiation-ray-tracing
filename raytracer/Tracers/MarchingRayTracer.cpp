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
#include <array>

RGBColor monte_carlo(Point3D x, Vector3D w, size_t number_of_samples, size_t number_of_secondary_samples, World* world, float max_t = std::numeric_limits<float>::max());

RGBColor MarchingRayTracer::trace_ray(const Ray& ray) const {
	return Tracer::trace_ray(ray);
}

auto emitter_location = Point3D{ 0, 0, 0 };
const RGBColor& glow_colour = RGBColor{ 2 / 255., 203 / 255., 213 / 255. };
float intensity = 1.0f;
constexpr float a = 0.09;
constexpr float s = 0.09;

#define dot *

// absorption coefficient at point p0
constexpr auto mu_a = [](float t) {
	return t * a;
};

// absorption coefficient at point t
constexpr auto mu_s = [](float t) {
	return t * s;
};

// extinction coefficient at point t
constexpr auto mu_t = [&](float t) {
	return mu_s(t) + mu_s(t);
};

// phase function
constexpr auto f_p = [&](Vector3D w, Vector3D w_bar) { return 1 / (4 * PI); };

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
	auto result = 1 / mu_t(100);
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
	static std::array<Vector3D, 1000> samples;
	static int i = 0;
	static int fill = 0;

	if (fill < samples.size()) {
		samples[fill] = S2_sample_raw();
		return samples[fill++];
	}
	else return samples[(size_t)(random_variable() * fill) % fill];
};

// radiance emitted from a volume at point x
auto Le = [&](Point3D x, Vector3D w, World* world) -> RGBColor {

	auto distance_from_source = (x - emitter_location).len_squared();

	float result = 0;
	result += 1.0f / distance_from_source;

	return result * intensity * glow_colour;
};

// incident radiance at point in direction w
auto Li = [&](Point3D x, Vector3D w, int number_of_samples, World* world) {
	//todo: should probably be recursive within the medium.

	auto emitter_direction = (x - emitter_location).hat();


	// cheat: treat the light source as a specular lobe
	//auto result = std::pow(std::max<float>(0.0f, emitter_direction dot w), 128);
	auto result = monte_carlo(x, w, number_of_samples, 0, world);
	return result * RGBColor{ 0 };
};



// in-scattering radiance
auto Ls = [&](Point3D x, Vector3D w, int number_of_samples, World* world) {

	RGBColor result{ 0 };
	for (int i = 0; i < number_of_samples; ++i)
	{
		Vector3D w_bar = S2_sample();
		result += f_p(w, w_bar) * Li(x, w_bar, number_of_samples, world);
	}
	return result / std::max(1, number_of_samples);
};

auto tau = [&](float t) {
	return t * 0.08;
};

//transmittance
auto T_r = [&](float t) {
	return std::exp(-tau(t));
};

auto p = [&](float t) {
	auto result = zeroth * std::exp(-zeroth * t);
	return result;
};

auto Sample = [&](Point3D x, Point3D y, float t, Vector3D w, World* world, int number_of_samples) {
	return T_r(t) / p(t) * (mu_a(t) * Le(y, w, world) + mu_s(t) * Ls(y, w, number_of_samples, world));
};

RGBColor monte_carlo(Point3D x, Vector3D w, size_t number_of_samples, size_t number_of_secondary_samples, World* world, float max_t)
{
	RGBColor result = { 0 };
	for (int i = 0; i < number_of_samples; i++) {
		auto t = sample_t();
		while (t > max_t) { t = sample_t(); }
		auto y = x + w * t;
		result += Sample(x, y, t, w, world, number_of_secondary_samples);
	}
	return result / std::max<size_t>(1, number_of_samples);
};

RGBColor MarchingRayTracer::trace_ray(const Ray ray, const int depth) const {

	// plan: cast a ray and see where it hits the scene...
	// then subdivide that distance by some value and sample backwards along the ray to accumulate
	// light in the medium.
	// the amount of light at a given point is a function of distance from the emitter

	ShadeRec sr(world_ptr->hit_objects(ray));

	auto [x, w] = ray;

	auto tone_map = [](const RGBColor& colour) {
		return std::pow(colour / (colour + RGBColor{ 1 }), Vector3D{ 1.0f / 2.2f });
	};

	if (sr.hit_an_object) {

		auto transmission = monte_carlo(x, w, 1, 1, world_ptr, sr.t) + Le(x + sr.t * w, -w, world_ptr);
		return (transmission);

	}
	else {

		auto transmission = monte_carlo(x, w, 0, 1, world_ptr);
		return (transmission);

	}
}

MarchingRayTracer::MarchingRayTracer(
	World* pWorld
) : Tracer(pWorld) {}
