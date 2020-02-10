/*
 * GlossySpecularMicrofacet.h
 *
 *  Created on: Jan 27, 2020
 *      Author: jackson.wiebe1
 */

#ifndef RAYTRACER_BRDFS_MICROFACET_H_
#define RAYTRACER_BRDFS_MICROFACET_H_

#include "BRDF.h"
#include "Constants.h"

enum class DistributionFunc {
	Beckmann, TrowbridgeReitz,
};

template<DistributionFunc DF>
class GlossySpecularMicrofacet: public BRDF {

public:

	GlossySpecularMicrofacet<DF>* clone(void) const {
		return (new GlossySpecularMicrofacet<DF>(*this));
	}

	RGBColor rho() const {
		return RGBColor(0);
	}

	RGBColor f(const ShadeRec& sr, const Vector3D& wo,
			const Vector3D& wi) const {

		Vector3D n = sr.normal;
		Vector3D wh = (wi + wo);
		wh.normalize();

		auto result = invPI * F(wi, wh) * G(wi, wo, wh) * D(n, wh)
				/ (4.0f * (n * wi) * (n * wo));

		return result;
	}

	void setRoughness(float roughness) {
		r = roughness;
	}

	void setSpecularColour(RGBColor color) {
		F0 = color;
	}

private:

	float r = 0.95;
	RGBColor F0 = RGBColor(1.00f, 0.71f, 0.29f);

	RGBColor F(const Vector3D& wi, const Vector3D& wh) const {

		return F0 + (RGBColor(1) - F0) * std::pow(1.0f - (wi * wh), 5);

	}

	double G(const Vector3D& wi, const Vector3D& wo, const Vector3D& wh) const {

		const float k = std::pow(r + 1.0f, 2) / 8.0f;
		auto G1 = [k](float dotProd) {
			return 1.0f / (dotProd * (1.0f - k) + k);
		};

		return G1(wh * wi) * G1(wh * wo);

	}

	double D(const Vector3D& n, const Vector3D& wh) const;
};

#endif /* RAYTRACER_BRDFS_MICROFACET_H_ */
