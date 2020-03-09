/*
 * Image.cpp
 *
 *  Created on: Feb 28, 2020
 *      Author: jackson.wiebe1
 */

#include <Image.h>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

Image::Image(const std::string &filename) {
    texel_data = std::unique_ptr<unsigned char>(
        stbi_load(
            filename.c_str(),
            &width,
            &height,
            &channels,
            STBI_rgb
        ));
}
