#include "Emissive.h"

#include <algorithm>
#include <vector>
#include <RayCast.h>
#include "Grid.h"
#include <functional>
#include <array>
#include <stb_image_write.h>


Emissive::Emissive(Mesh* mesh) :
	Material() {

}


RGBColor Emissive::shade(ShadeRec& sr) {

	// at the surface of an emissive material the colour should be the intensity of the light at that distance.

	RGBColor result{ 0 };

	for (auto&& emission_source : sr.w->lights) {
		
		result += emission_source->L(sr);
	}

	return result;

}