//
// Created by Jackson Cougar Wiebe on 3/15/2020.
//

#ifndef WXRAYTRACER_MARCHINGRAYTRACER_HPP
#define WXRAYTRACER_MARCHINGRAYTRACER_HPP


#include "Tracer.h"
#include "RGBColor.h"
#include "Ray.h"

class MarchingRayTracer : public Tracer {
public:
    MarchingRayTracer(World *pWorld);
    [[nodiscard]] RGBColor trace_ray(const Ray &ray) const override;
    [[nodiscard]] RGBColor trace_ray(const Ray ray, const int depth) const override;
};


#endif //WXRAYTRACER_MARCHINGRAYTRACER_HPP
