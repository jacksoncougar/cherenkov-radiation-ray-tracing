/*
 * GlossySpecularMicrofacet.h
 *
 *  Created on: Jan 27, 2020
 *      Author: jackson.wiebe1
 */

#ifndef RAYTRACER_BRDFS_GLOSSYSPECULARMICROFACET_H_
#define RAYTRACER_BRDFS_GLOSSYSPECULARMICROFACET_H_

#include "BRDF.h"

class GlossySpecularMicrofacet: public BRDF {

public:

	GlossySpecularMicrofacet();

	GlossySpecularMicrofacet(const GlossySpecularMicrofacet& m);

	/**
	 * Fresnel reflectance function
	 */
	RGBColor F(const Vector3D& wi, const Vector3D& wh) const;

	/**
	 * Microfacet light occlusion function
	 */
	float G(const Vector3D& wi, const Vector3D& wo,
			const Vector3D& wh) const;

	/**
	 * Microfacet orientation function
	 */
	RGBColor D(const Vector3D& n, const Vector3D& wh) const;

	RGBColor rho() const;

	RGBColor f(const ShadeRec& sr, const Vector3D& wo,
			const Vector3D& wi) const;

	GlossySpecularMicrofacet* clone(void) const;

private:

	float		kd;
	RGBColor 	cd;
};

#endif /* RAYTRACER_BRDFS_GLOSSYSPECULARMICROFACET_H_ */
