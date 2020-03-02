//
// Created by root on 2/3/20.
//

#pragma once
#ifndef WXRAYTRACER_ATTRIBUTE_HPP
#define WXRAYTRACER_ATTRIBUTE_HPP

#include <nmemory>
#include "BRDF.h"
#include "ImageTexture.h"

class Attribute : public BRDF {
public:
    RGBColor f(const ShadeRec &sr, const Vector3D &wo, const Vector3D &wi) const override {
        return texture->sample(sr);
    }

private:
    std::shared_ptr <ImageTexture> texture;
};


#endif //WXRAYTRACER_ATTRIBUTE_HPP
