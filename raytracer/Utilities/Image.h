/*
 * Image.h
 *
 *  Created on: Feb 28, 2020
 *      Author: jackson.wiebe1
 */

#pragma once

#ifndef RAYTRACER_UTILITIES_IMAGE_H_
#define RAYTRACER_UTILITIES_IMAGE_H_

#include "RGBColor.h"
#include "ShadeRec.h"

#include <cassert>
#include <vector>
#include <string>
#include <memory>

class Image {

public:

    std::unique_ptr<unsigned char> texel_data;
    int width{};
    int height{};
    int channels{};

    Image(const std::string &filename);

    template<typename T>
    T sample(int x, int y) {

        auto bits_per_texel = channels * sizeof(char);
        auto scanline = width * bits_per_texel;
        auto offset = x + y * scanline;

        assert(channels == 3);

        auto r = *(texel_data.get() + offset + 0) /
                 static_cast<float>(std::numeric_limits<unsigned char>::max());
        auto g = *(texel_data.get() + offset + 1) /
                 static_cast<float>(std::numeric_limits<unsigned char>::max());
        auto b = *(texel_data.get() + offset + 2) /
                 static_cast<float>(std::numeric_limits<unsigned char>::max());

        return {r, g, b};
    }

};

#endif /* RAYTRACER_UTILITIES_IMAGE_H_ */
