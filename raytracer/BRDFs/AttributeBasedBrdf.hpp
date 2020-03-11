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

template<>
[[nodiscard]] float AttributeBasedBrdf<ViewDependantFunction::NearSilhouette>::D(const ShadeRec &sr,
                                                                                 const Vector3D &wo,
                                                                                 const Vector3D &wi) const {
    return std::pow(sr.normal * wo, r);
}

template<>
[[nodiscard]] float AttributeBasedBrdf<ViewDependantFunction::Highlights>::D(const ShadeRec &sr,
                                                                             const Vector3D &wo,
                                                                             const Vector3D &wi) const {
    auto wr = wi - 2 * (wi * sr.normal) * sr.normal; // reflection
    return std::pow(wo * wr, r);
}

template<>
[[nodiscard]] float AttributeBasedBrdf<ViewDependantFunction::Depth>::D(const ShadeRec &sr,
                                                                        const Vector3D &wo,
                                                                        const Vector3D &wi) const {
    return std::pow(sr.normal * wo, r);
}


#endif //WXRAYTRACER_ATTRIBUTEBASEDBRDF_HPP
