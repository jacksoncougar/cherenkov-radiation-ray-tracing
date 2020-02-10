#include "microfacet.h"

// ---------------------------------------------------------------- default constructor

Microfacet::Microfacet(void) :
		Material(), ambient_brdf(new Lambertian), diffuse_brdf(new Lambertian), specular_brdf(
				new GlossySpecularMicrofacet<DistributionFunc::Beckmann>) {
}

// ---------------------------------------------------------------- copy constructor

Microfacet::Microfacet(const Microfacet& m) :
		Material(m) {
	if (m.ambient_brdf)
		ambient_brdf = m.ambient_brdf->clone();
	else
		ambient_brdf = NULL;

	if (m.diffuse_brdf)
		diffuse_brdf = m.diffuse_brdf->clone();
	else
		diffuse_brdf = NULL;

	if (m.specular_brdf)
		specular_brdf = m.specular_brdf->clone();
	else
		specular_brdf = NULL;
}

// ---------------------------------------------------------------- clone

Material*
Microfacet::clone(void) const {
	return (new Microfacet(*this));
}

// ---------------------------------------------------------------- assignment operator

Microfacet&
Microfacet::operator=(const Microfacet& rhs) {
	if (this == &rhs)
		return (*this);

	Material::operator=(rhs);

	if (ambient_brdf) {
		delete ambient_brdf;
		ambient_brdf = NULL;
	}

	if (rhs.ambient_brdf)
		ambient_brdf = rhs.ambient_brdf->clone();

	if (diffuse_brdf) {
		delete diffuse_brdf;
		diffuse_brdf = NULL;
	}

	if (rhs.diffuse_brdf)
		diffuse_brdf = rhs.diffuse_brdf->clone();

	return (*this);
}

// ---------------------------------------------------------------- destructor

Microfacet::~Microfacet(void) {

	if (ambient_brdf) {
		delete ambient_brdf;
		ambient_brdf = NULL;
	}

	if (diffuse_brdf) {
		delete diffuse_brdf;
		diffuse_brdf = NULL;
	}
}

// ---------------------------------------------------------------- shade

RGBColor Microfacet::shade(ShadeRec& sr) {

	Vector3D wo = -sr.ray.d;
	RGBColor L = ambient_brdf->rho(sr, wo) * sr.w.ambient_ptr->L(sr);
	int num_lights = sr.w.lights.size();

	for (int j = 0; j < num_lights; j++) {
		Vector3D wi = sr.w.lights[j]->get_direction(sr);
		float ndotwi = sr.normal * wi;

		if (ndotwi > 0.0) {
			L += (diffuse_brdf->f(sr, wo, wi) + specular_brdf->f(sr, wo, wi))
					* sr.w.lights[j]->L(sr) * ndotwi;
		}
	}

	return (L);
}

