//
// Created by root on 2/3/20.
//

#pragma once
#ifndef WXRAYTRACER_SVATTRIBUTEBASEDMAPPING_HPP
#define WXRAYTRACER_SVATTRIBUTEBASEDMAPPING_HPP

#include <utility>
#include <algorithm>

#include "AttributeBasedBrdf.hpp"
#include "Material.h"

struct svAttributeBasedMapping : public Material {
    AttributeBasedBrdf<NearSilhouette> brdf;
public:
    explicit svAttributeBasedMapping(std::shared_ptr<ImageTexture> texture) : brdf(
            std::move(texture)) {}

    [[nodiscard]] svAttributeBasedMapping *clone() const override {
        return new svAttributeBasedMapping(*this);
    }

    ~svAttributeBasedMapping() override = default;

    float r() { return brdf.r; }

    void r(float value) { brdf.r = std::max(value, 0.0f); }

    RGBColor shade(ShadeRec &sr) override {
        Vector3D wo = -sr.ray.d;
        RGBColor L = brdf.rho(sr, wo) * sr.w.ambient_ptr->L(sr);
        auto num_lights = sr.w.lights.size();

        for (int j = 0; j < num_lights; j++) {
            Vector3D wi = sr.w.lights[j]->get_direction(sr);
            if (sr.normal * wi > 0.0)
                L += brdf.f(sr, wo, wi);
        }

        return (L);
    }
};


#endif //WXRAYTRACER_SVATTRIBUTEBASEDMAPPING_HPP
