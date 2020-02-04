/*
 * GlossySpecularMicrofacet.cpp
 *
 *  Created on: Jan 27, 2020
 *      Author: jackson.wiebe1
 */

#include <GlossySpecularMicrofacet.h>
#include <cmath>

GlossySpecularMicrofacet::GlossySpecularMicrofacet() {
}

GlossySpecularMicrofacet::GlossySpecularMicrofacet(
		const GlossySpecularMicrofacet& m) :
		BRDF(m) {
}

RGBColor GlossySpecularMicrofacet::F(const Vector3D& wi,
		const Vector3D& wh) const {

	RGBColor F0;

	return F0 + (RGBColor(-1) + F0) * std::pow(1.0f - (wi * wh), 5);

}

float GlossySpecularMicrofacet::G(const Vector3D& wi, const Vector3D& wo,
		const Vector3D& wh) const {

	float r = 0.5f;

	auto G1 = [k = std::pow(r + 1, 2) / 8](auto dotProd) {
		return 1 / (dotProd * (1 - k) + k);
	};

	return G1(wh * wi) * G1(wh * wo);

}

RGBColor GlossySpecularMicrofacet::D(const Vector3D& n,
		const Vector3D& wh) const {

	float a = std::acos(n * wh);
	float r = 0.5;
	// Beckmann
	return std::exp(-std::pow(std::tan(a) / r, 2))
			/ (std::pow(r, 2) * std::pow(cos(a), 4));
}

RGBColor GlossySpecularMicrofacet::rho() const {
	return (kd * cd);
}

RGBColor GlossySpecularMicrofacet::f(const ShadeRec& sr, const Vector3D& wo,
		const Vector3D& wi) const {

	const Vector3D& n = sr.normal;
	const Vector3D wh = wi + wo;

	return (kd * cd) + F(wi, wh) * G(wi, wo, wh) * D(n, wh) / (4 * (wo * n) * (wi * n));
}

GlossySpecularMicrofacet* GlossySpecularMicrofacet::clone(void) const {
	return (new GlossySpecularMicrofacet(*this));
}
