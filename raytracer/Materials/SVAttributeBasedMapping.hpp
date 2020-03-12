//
// Created by root on 2/3/20.
//

#pragma once
#ifndef WXRAYTRACER_SVATTRIBUTEBASEDMAPPING_HPP
#define WXRAYTRACER_SVATTRIBUTEBASEDMAPPING_HPP

#include <utility>
#include <algorithm>
#include <iostream>

#include "AttributeBasedBrdf.hpp"
#include "Material.h"

template<typename B>
struct svAttributeBasedMapping : public Material {
    B brdf;
public:
    void set_attribute_image(std::shared_ptr<ImageTexture> texture) {
        brdf.set_texture(texture);
    }

    [[nodiscard]] svAttributeBasedMapping *clone() const override {
        return new svAttributeBasedMapping(*this);
    }

    ~svAttributeBasedMapping() override = default;

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


struct svHighlightsMaterial : public svAttributeBasedMapping<MetalHighlightsBrdf> {
    float s() { return brdf.s; }

    void s(float value) {
        float s_value = std::max(value, 1.000001f);
        std::cout << "Using s = " << s_value << std::endl;
        brdf.s = s_value;
    }
};


struct svDepthMaterial : public svAttributeBasedMapping<DepthBrdf> {
    float z_min() { return brdf.z_min; }

    void z_min(float value) {
        float z_min_value = value;
        std::cout << "Using z_min = " << z_min_value << std::endl;
        brdf.z_min = z_min_value;
    }

    float r() { return brdf.r; }

    void r(float value) {
        float r_value = std::max(value, 0.0f);
        std::cout << "Using r = " << r_value << std::endl;
        brdf.r = r_value;
    }
};

struct svSilhouetteMaterial : public svAttributeBasedMapping<NearSilhouetteBrdf> {
    float r() { return brdf.r; }

    void r(float value) {
        float r_value = std::max(value, 0.0f);
        std::cout << "Using r = " << r_value << std::endl;
        brdf.r = r_value;
    }
};

#endif //WXRAYTRACER_SVATTRIBUTEBASEDMAPPING_HPP
