//
// Created by root on 2/3/20.
//

#pragma once
#ifndef WXRAYTRACER_SVATTRIBUTEBASEDMAPPING_HPP
#define WXRAYTRACER_SVATTRIBUTEBASEDMAPPING_HPP

#include "Attribute.hpp"
#include "Material.h"

class svAttributeBasedMapping : public Material {
    Attribute brdf;
    ImageTexture texture;
public:
    svAttributeBasedMapping *clone() const override { return new svAttributeBasedMapping(*this); }

    ~svAttributeBasedMapping() override { /*...*/ }

    RGBColor shade(ShadeRec &sr) override {
        Vector3D wo = -sr.ray.d;
        RGBColor L = brdf.rho(sr, wo) * sr.w.ambient_ptr->L(sr);
        auto num_lights = sr.w.lights.size();

        for (int j = 0; j < num_lights; j++) {
            Vector3D wi = sr.w.lights[j]->get_direction(sr);
            float ndotwi = sr.normal * wi;

            if (ndotwi > 0.0)
                L += brdf.f(sr, wo, wi) * sr.w.lights[j]->L(sr) * ndotwi;
        }

        return (L);
    }
};


#endif //WXRAYTRACER_SVATTRIBUTEBASEDMAPPING_HPP
