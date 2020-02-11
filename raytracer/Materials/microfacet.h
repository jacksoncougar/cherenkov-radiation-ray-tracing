#ifndef __MICROFACET__
#define __MICROFACET__

#include <Microfacet.h>
#include "Material.h"
#include "Lambertian.h"

//----------------------------------------------------------------------------- class Matte

class Microfacet: public Material {
public:

	Microfacet(void);

	Microfacet(const Microfacet& m);

	virtual Material*
	clone(void) const;

	Microfacet&
	operator=(const Microfacet& rhs);

	~Microfacet(void);

	void set_ka(const float k);

	void set_kd(const float k);

	void set_cd(const RGBColor k);

	void set_specular_colour(const RGBColor);
	void set_roughness(const float);

	virtual RGBColor
	shade(ShadeRec& sr);

private:

	Lambertian* ambient_brdf;
	Lambertian* diffuse_brdf;
	GlossySpecularMicrofacet<DistributionFunc::TrowbridgeReitz>* specular_brdf;
};

inline void Microfacet::set_ka(const float ka) {
	ambient_brdf->set_kd(ka);
}

inline void Microfacet::set_kd(const float kd) {
	diffuse_brdf->set_kd(kd);
}

inline void Microfacet::set_cd(const RGBColor c) {
	ambient_brdf->set_cd(c);
	diffuse_brdf->set_cd(c);
}

inline void Microfacet::set_specular_colour(const RGBColor color) {
	specular_brdf->setSpecularColour(color);
}

inline void Microfacet::set_roughness(const float roughness) {
	specular_brdf->setRoughness(roughness);
}

#endif
