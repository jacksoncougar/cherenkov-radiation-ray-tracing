#pragma once

#include <cmath>
#include "Image.h"
#include "RGBColor.h"
#include "ShadeRec.h"


class ImageTexture {
public:
	std::shared_ptr<Image> data;
	std::shared_ptr<void*> mapping;


	explicit ImageTexture(int width, int height, int number_of_channels) {
		data = std::make_shared<Image>(width, height, number_of_channels);
	}
	explicit ImageTexture(std::shared_ptr<Image> data) : data(std::move(data)) {}

	RGBColor sample(const ShadeRec& sr) {
		int x = static_cast<int>(sr.u * (data->width - 1.0));
		// invert u because data is laid out from top to bottom in image...
		int y = static_cast<int>((1.0 - sr.v) * (data->height - 1.0));

		return data->sample<RGBColor>(x, y);
	}

	void set(float u, float v, RGBColor color) { 



		auto quantize = [](float value) {
			return static_cast<std::uint8_t>(std::numeric_limits<std::uint8_t>::max() * value);
		};

		data->set(u, v, quantize(color.r), quantize(color.g), quantize(color.b));
	}

};