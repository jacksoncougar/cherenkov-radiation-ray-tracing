//
// Created by root on 2/3/20.
//

#pragma once
#ifndef WXRAYTRACER_ATTRIBUTEBASEDBRDF_HPP
#define WXRAYTRACER_ATTRIBUTEBASEDBRDF_HPP

#include <memory>
#include "BRDF.h"
#include "ImageTexture.hpp"
#include "RGBColor.h"
#include "ShadeRec.h"

enum ViewDependantFunction {
    NearSilhouette, Highlights, Depth
};

template<ViewDependantFunction>
class AttributeBasedBrdf : public BRDF {
public:
    explicit AttributeBasedBrdf(std::shared_ptr<ImageTexture> image) : texture(std::move(image)) {}

    float r = 21.5;

    [[nodiscard]] float D(const ShadeRec &sr, const Vector3D &wo, const Vector3D &wi) const {
        return std::pow(sr.normal * wo, r);
    }

    [[nodiscard]] RGBColor f(const ShadeRec &sr, const Vector3D &wo,
                             const Vector3D &wi) const override {
        auto n_dot_l = sr.normal * wi;
        auto d = D(sr, wo, wi);
        auto sr_copy = sr;
        sr_copy.u = n_dot_l;
        sr_copy.v = d;
        return texture->sample(sr_copy);
    }

    [[nodiscard]] BRDF *clone() const override {
        return nullptr;
    }

private:
    std::shared_ptr<ImageTexture> texture;
};

#endif //WXRAYTRACER_ATTRIBUTEBASEDBRDF_HPP
