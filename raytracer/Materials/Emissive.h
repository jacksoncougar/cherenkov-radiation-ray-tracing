#ifndef __EMISSIVE__
#define __EMISSIVE__

#include <Emissive.h>
#include "Material.h"
#include "Lambertian.h"
#include <memory>
#include <ImageTexture.hpp>


class Emissive : public Material {
public:

	Emissive(Mesh* mesh);
	std::unique_ptr<ImageTexture> image = std::make_unique<ImageTexture>(512, 512, 3);

	virtual RGBColor	shade(ShadeRec& sr);

private:
};


#endif
