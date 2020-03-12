//
// Created by root on 2/3/20.
//

#pragma once
#ifndef WXRAYTRACER_ATTRIBUTEBASEDBRDF_HPP
#define WXRAYTRACER_ATTRIBUTEBASEDBRDF_HPP

#include <memory>
#include <algorithm>
#include "BRDF.h"
#include "ImageTexture.hpp"
#include "RGBColor.h"
#include "ShadeRec.h"

enum ViewDependantFunction {
    NearSilhouette, Highlights, Depth
};

class NearSilhouetteBrdf : public BRDF {
public:
    float r = 1.5;

    [[nodiscard]] float D(const ShadeRec &sr, const Vector3D &wo, const Vector3D &wi) const {
        return std::pow(sr.normal * wo, r);
    }

    void set_texture(std::shared_ptr<ImageTexture> texture) {
        this->texture = texture;
    }

    [[nodiscard]] RGBColor f(
        const ShadeRec &sr, const Vector3D &wo, const Vector3D &wi
    ) const override {
        if (!texture) return {0};

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

class MetalHighlightsBrdf : public BRDF {
public:
    float s = 1.5;

    [[nodiscard]] float D(const ShadeRec &sr, const Vector3D &wo, const Vector3D &wi) const {
        auto wr = 2 * (sr.normal * wi) * sr.normal - wi;
        return std::pow(wr * wo, s);
    }

    void set_texture(std::shared_ptr<ImageTexture> texture) {
        this->texture = texture;
    }

    [[nodiscard]] RGBColor f(
        const ShadeRec &sr, const Vector3D &wo, const Vector3D &wi
    ) const override {
        if (!texture) return {0};

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

class DepthBrdf : public BRDF {
public:
    float r = 5.5;
    float z_min = 0.5;

    [[nodiscard]] float D(const ShadeRec &sr, const Vector3D &wo, const Vector3D &wi) const {
        auto z = sr.t;
        auto z_max = r * z_min;
        auto result = 1.0 - std::logf(z / z_min) / std::logf(r);
        return std::clamp(result, 0.0, 1.0); // sanitize return values in uv range.
    }

    void set_texture(std::shared_ptr<ImageTexture> texture) {
        this->texture = texture;
    }

    [[nodiscard]] RGBColor f(
        const ShadeRec &sr, const Vector3D &wo, const Vector3D &wi
    ) const override {
        if (!texture) return {0};

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
