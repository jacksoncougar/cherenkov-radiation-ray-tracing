#pragma once

#include <cmath>
#include "Image.h"
#include "RGBColor.h"
#include "ShadeRec.h"

class ImageTexture {
    std::shared_ptr<Image> data;
    std::shared_ptr<void *> mapping;

public:
    explicit ImageTexture(std::shared_ptr<Image> data) : data(std::move(data)) {}

    RGBColor sample(const ShadeRec &sr) {
        int x = static_cast<int>(sr.u * (data->width - 1.0));
        // invert u because data is laid out from top to bottom in image...
        int y = static_cast<int>((1.0 - sr.v) * (data->height - 1.0));

        return data->sample<RGBColor>(x, y);
    }
};