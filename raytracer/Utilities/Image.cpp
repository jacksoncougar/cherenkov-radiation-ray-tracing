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

Image::Image(const std::string& filename) {

	auto raw_ptr = static_cast<std::uint8_t*>(stbi_load(
		filename.c_str(),
		&width,
		&height,
		&channels,
		STBI_rgb
	));
	texel_data = std::vector<std::uint8_t>(raw_ptr, raw_ptr + (width * height * channels * sizeof(uint8_t)));
}
