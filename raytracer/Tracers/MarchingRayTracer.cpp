//
// Created by Jackson Cougar Wiebe on 3/15/2020.
//

#include "MarchingRayTracer.hpp"
#include <ShadeRec.h>
#include <cmath>
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

    if (sr.hit_an_object) {

        auto sample_point = ray.o + ray.d * sr.t;
        auto distance_to_emitter = std::pow(sample_point.distance(emitter_location), 2);
        sr.ray = ray;            // used for specular shading
        auto colour = glow_colour * intensity * 1.0f / distance_to_emitter;
        auto tone = std::pow(colour / (colour + RGBColor{1}), Vector3D{1.0f / 2.2f});
        return (tone) + sr.material_ptr->shade(sr);

    } else {

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

MarchingRayTracer::MarchingRayTracer(World *pWorld) : Tracer(pWorld) {}
