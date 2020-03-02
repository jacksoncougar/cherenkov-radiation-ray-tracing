#pragma once

#include "Image.h"
#include "RGBColor.h"
#include "ShadeRec.h"

class ImageTexture
{
	std::shared_ptr<Image> data;
	std::shared_ptr<void*> mapping;

	RGBColor sample(const ShadeRec& sr)
	{
		int x = sr.u * (data.width - 1);
		int y = sr.v * (data.height - 1);

		return data.sample<RGBColor>(x, y);
	}
};