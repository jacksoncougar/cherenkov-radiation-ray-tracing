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

RGBColor MarchingRayTracer::trace_ray(const Ray& ray) const {
	return Tracer::trace_ray(ray);
}

RGBColor MarchingRayTracer::trace_ray(const Ray ray, const int depth) const {

	// plan: cast a ray and see where it hits the scene...
	// then subdivide that distance by some value and sample backwards along the ray to accumulate
	// light in the medium.
	// the amount of light at a given point is a function of distance from the emitter

	ShadeRec sr(world_ptr->hit_objects(ray));

	auto emitter_location = Point3D{ 0, 0, 0 };
	const RGBColor& glow_colour = RGBColor{ 0.15, 0.182, .537 };
	float intensity = 30.10f;

#define dot *

	// visibility function
	auto V = [](Vector3D p0, Vector3D p1, ShadeRec* sr = nullptr) {
		return 1.0; // assume everything is visible
	};

	// foreshortening function
	auto D = [](Vector3D p0, Vector3D p1, ShadeRec* sr = nullptr) {

		// we expect a shader record only if the point is on a surface.
		// otherwise we assume that we are in a medium.

		if (sr) {
			auto w = Normal(p1 - p0);
			return sr->normal dot w;
		}
		else return 1.0;
	};

	// geometric function
	auto G = [V, D](Vector3D p0, Vector3D p1) {
		return V(p0, p1) * D(p0, p1) * D(p1, p0) / (p0 - p1).len_squared();
	};



	// absorption coefficient at point p0
	constexpr auto mu_a = [](float t) {
		return t * 0.026094;
	};

	// absorption coefficient at point t
	constexpr auto mu_s = [](float t) {
		return t * 0.4201;
	};



	// extinction coefficient at point t
	auto mu_t = [&](float t) {
		return mu_s(t) + mu_s(t);
	};

	//scattering coefficient at point p0
	// this term also controls how much light should scatter "in", which can be used to offset the
	// geometric terms distance falloff.
	auto scattering_at = [emitter_location](Vector3D p0) {
		return 0.5201;
	};

	// radiance emitted from a volume at point x
	auto Le = [&](Point3D x, Vector3D w) -> RGBColor {
		float result = intensity * 1.0f / ((x - emitter_location).len_squared());
		return result * glow_colour;
	};

	// incident radiance at point in direction w
	auto Li = [&](Point3D x, Vector3D w) {
		//todo: should probably be recursive within the medium.

		auto emitter_direction = (x - emitter_location).hat();

		// cheat: treat the light source as a specular lobe
		auto result = std::pow(std::max<float>(0.0f, emitter_direction dot w), 128);
		return result * glow_colour;
	};

	// phase function
	constexpr auto f_p = [&](Vector3D w, Vector3D w_bar) { return 1 / (4 * PI); };

	auto random_variable = []() {
		static std::random_device rd;
		static std::mt19937 e2(rd());
		static std::uniform_real_distribution<> dist(0, 1);
		return dist(e2);
	};


	auto zeroth = [&]() {
		float moment = 0;
		for (int i = 0; i < 100; i++)
		{
			moment += mu_t(i) / 100;
		}
		return 1 / moment;
	};

	auto sample_t = [&]() {
		auto xi = random_variable();
		auto result = -std::log(1 - xi) / zeroth();
		return result;
	};

	auto S2_sample = [&]() {
		float u = random_variable();
		float v = random_variable();
		float theta = 2 * PI * u;
		float phi = std::acos(2 * v - 1);
		float x = std::sin(theta) * std::cos(phi);
		float y = std::sin(theta) * std::sin(phi);
		float z = std::cos(theta);

		return Vector3D(x, y, z);
	};

	// in-scattering radiance
	auto Ls = [&](Point3D x, Vector3D w, int number_of_samples) {

		RGBColor result{ 0 };
		for (int i = 0; i < number_of_samples; ++i)
		{
			Vector3D w_bar = S2_sample();
			result += f_p(w, w_bar) * Li(x, w_bar);
		}
		return result / number_of_samples;
	};

	auto tau = [&](float t) {
		return t * 0.111;
	};

	//transmittance
	auto T_r = [&](float t) {
		return std::exp(-tau(t));
	};



	auto p = [&](float t) {
		auto result = zeroth() * std::exp(-zeroth() * t);
		return result;
	};


	auto Sample = [&](Point3D x, Point3D y, float t, Vector3D w) {
		return /*T_r(t) / p(t) **/ (mu_a(t) * Le(y, w) + mu_s(t) * Ls(y, w, 16));
	};



	auto monte_carlo = [&](Point3D x, Vector3D w, size_t number_of_samples)
	{
		RGBColor result = { 0 };
		for (int i = 0; i < number_of_samples; i++) {
			const float max_depth = 1;
			auto t = sample_t();
			auto y = x + w * t;
			result += Sample(x, y, t, w);
		}
		return result / number_of_samples;
	};

	auto [x, w] = ray;

	auto tone_map = [](const RGBColor& colour) {
		return std::pow(colour / (colour + RGBColor{ 1 }), Vector3D{ 1.0f / 2.2f });
	};

	if (sr.hit_an_object) { 

		float maximum = 1 / zeroth() * 2;

		return { sr.t / maximum,sr.t / maximum,sr.t / maximum };

	}
	else {

		auto transmission = monte_carlo(x, w, 4);
		return (transmission);

	}
}

MarchingRayTracer::MarchingRayTracer(
	World* pWorld
) : Tracer(pWorld) {}
