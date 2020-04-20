//
// Created by Jackson Cougar Wiebe on 3/15/2020.
//

#include "MarchingRayTracer.hpp"
#include <ShadeRec.h>
#include <cmath>
#include <algorithm>
#include "World.h"
#include "Material.h"

RGBColor MarchingRayTracer::trace_ray(const Ray &ray) const {
    return Tracer::trace_ray(ray);
}

RGBColor MarchingRayTracer::trace_ray(const Ray ray, const int depth) const {

    // plan: cast a ray and see where it hits the scene...
    // then subdivide that distance by some value and sample backwards along the ray to accumulate
    // light in the medium.
    // the amount of light at a given point is a function of distance from the emitter

    ShadeRec sr(world_ptr->hit_objects(ray));

    auto emitter_location = Point3D{0, 0, 0};
    const RGBColor &glow_colour = RGBColor{0.15, 0.182, .537};
    float intensity = 15.10f;

#define dot *


    // visibility function
    auto V = [](Vector3D p0, Vector3D p1, ShadeRec *sr = nullptr) {
        return 1.0;
    };

    // foreshortening function
    auto D = [](Vector3D p0, Vector3D p1, ShadeRec *sr = nullptr) {
        if (sr) {
            auto w = Normal(p1 - p0);
            return sr->normal dot w;
        } else return 1.0;
    };

    // geometric function
    auto G = [V, D](Vector3D p0, Vector3D p1) {
        return V(p0, p1) * (D(p0, p1) * D(p1, p0)) / (p0 - p1).len_squared();
    };

    // extinction coefficient at point p0
    auto mu_t = [](float t) {
        return t * 0.1094;
    };

    //scattering coefficient at point p0
    // this term also controls how much light should scatter "in", which can be used to offset the
    // geometric terms distance falloff.
    auto scattering_at = [emitter_location](Vector3D p0) {
        return 0.0201;
    };

    // radiance emitted from a volume at point p0
    auto Le = [emitter_location](Vector3D p0, Vector3D p1) {
        auto result = (p0 - emitter_location).len_squared() * 0.005;
        return result;
    };

    //transmittance
    auto T_r = [mu_t](Vector3D p0, Vector3D p1) {
        auto t = (p0 - p1).length();
        auto transmittance = mu_t(t);
        return std::exp(-transmittance);
    };


    auto T = [emitter_location, G, T_r, Le, scattering_at](auto ray, ShadeRec sr) {
        auto surface_hit_location = ray.o + ray.d * sr.t;

        auto step_size = 1.00f; // sample every 1 units
        auto number_of_samples = sr.hit_an_object ? static_cast<int>(sr.t / step_size) : 100;

        auto p0 = ray.o + ray.d * 0 * step_size;
        auto p1 = ray.o + ray.d * 1 * step_size;

        auto result = [G, T_r, scattering_at, Le](auto p0, auto p1) {
            auto $if = [](auto condition, auto function) {
                return condition ? function : 1.0f;
            };
            bool geometry = true;
            bool transmission = true;
            bool scatter = true;
            bool emission = false;

            return $if(geometry, G(p0, p1)) * $if(transmission, T_r(p0, p1)) *
                   $if(scatter, scattering_at(p0)) * $if(emission, Le(p0, p1));
        };

        auto T_result = result(p0, p1);
        int i;
        for (i = 1; i < number_of_samples; ++i) {
            p0 = ray.o + ray.d * i * step_size;
            p1 = ray.o + ray.d * (i + 1) * step_size;
            T_result *= result(p0, p1);
        }

        p0 = ray.o + ray.d * i * step_size;
        p1 = ray.o + ray.d * sr.t;
        T_result *= result(p0, p1);

        return T_result / 20000.0;
    };

    if (sr.hit_an_object) {

        auto sample_point = ray.o + ray.d * sr.t;
        auto distance_to_emitter = std::pow(sample_point.distance(emitter_location), 2);
        sr.ray = ray;            // used for specular shading
        auto colour = glow_colour * intensity * 1.0f / distance_to_emitter;
        auto tone = std::pow(colour / (colour + RGBColor{1}), Vector3D{1.0f / 2.2f});
        auto tone_map = [](const RGBColor &colour) {
            return std::pow(colour / (colour + RGBColor{1}), Vector3D{1.0f / 2.2f});
        };

        auto transmission = T(ray, sr);
        auto Leo = Le(sample_point, sample_point);
        auto radiance = transmission * 100.0;
        return transmission;

    } else {

        auto transmission = T(ray, sr);
        return transmission;
        float ambient_max = 10.0f;
        float gather = 0;

        auto amount_of_samples = 200.0f;
        for (auto s = 0; s < amount_of_samples; ++s) {
            auto length_percent = static_cast<float>(s) / amount_of_samples;
            auto sample_point = ray.o + ray.d * length_percent * ambient_max;
            auto distance_to_emitter = std::powf((float) sample_point.distance(emitter_location),
                                                 2.0f
            );
            gather += intensity * 1.0f / distance_to_emitter;
        }
        gather /= amount_of_samples;

        auto colour = glow_colour * gather;
        auto tone = std::pow(colour / (colour + RGBColor{1}), Vector3D{1.0f / 2.2f});
        return (tone + world_ptr->background_color);

    }
}

MarchingRayTracer::MarchingRayTracer(
    World *pWorld
) : Tracer(pWorld) {}
