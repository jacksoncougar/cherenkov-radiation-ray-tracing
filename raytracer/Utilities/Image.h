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
#include <iostream>

class Image {

public:

	int width{};
	int height{};
	int channels{};
	std::vector<std::uint8_t> texel_data;

	Image(const std::string& filename);

	Image(int width, int height, int number_of_channels)
		: width(width), height(height), channels(number_of_channels), texel_data(width* height* channels * sizeof(std::uint8_t))
	{

	}

	template<typename T>
	T sample(int x, int y) {

		auto stride = channels * sizeof(std::uint8_t);
		auto offset = y * width * stride + x * stride;

		assert(channels == 3);

		auto r = texel_data[offset + 0] /
			static_cast<float>(std::numeric_limits<std::uint8_t>::max());
		auto g = texel_data[offset + 1] /
			static_cast<float>(std::numeric_limits<std::uint8_t>::max());
		auto b = texel_data[offset + 2] /
			static_cast<float>(std::numeric_limits<std::uint8_t>::max());

		return { r, g, b };
	}

	void set(int x, int y, std::uint8_t r, std::uint8_t g, std::uint8_t b) {

		auto stride = channels * sizeof(std::uint8_t);
		auto offset = y * width * stride + x * stride;

		texel_data[offset + 0] = r;
		texel_data[offset + 1] = g;
		texel_data[offset + 2] = b;
	}

};

#endif /* RAYTRACER_UTILITIES_IMAGE_H_ */
