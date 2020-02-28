/*
 * Image.h
 *
 *  Created on: Feb 28, 2020
 *      Author: jackson.wiebe1
 */

#ifndef RAYTRACER_UTILITIES_IMAGE_H_
#define RAYTRACER_UTILITIES_IMAGE_H_

#include "RGBColor.h"
#include <vector>
#include <string>
#include <memory>

class Image {

	std::unique_ptr<char> texel_data;
	int width;
	int height;
	int channels;

public:
	Image(std::string filename);

	RGBColor sample(int u, int v) {

		auto offset = u * width + v * channels * sizeof(char);

		auto r = *(texel_data.get() + offset + 0) / 255.0f;
		auto g = *(texel_data.get() + offset + 1) / 255.0f;
		auto b = *(texel_data.get() + offset + 2) / 255.0f;

		return {r,g,b};
	}

};

#endif /* RAYTRACER_UTILITIES_IMAGE_H_ */
