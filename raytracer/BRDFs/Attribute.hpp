//
// Created by root on 2/3/20.
//

#pragma once
#ifndef WXRAYTRACER_ATTRIBUTE_HPP
#define WXRAYTRACER_ATTRIBUTE_HPP

#include <memory>
#include "BRDF.h"
#include "ImageTexture.hpp"
#include "RGBColor.h"
#include "ShadeRec.h"

class Attribute : public BRDF {
public:
    RGBColor f(const ShadeRec &sr, const Vector3D &wo, const Vector3D &wi) const override {
        return texture->sample(sr);
    }

    BRDF *clone(void) const override {
        return nullptr;
    }

private:
    std::shared_ptr<ImageTexture> texture;
};


#endif //WXRAYTRACER_ATTRIBUTE_HPP
