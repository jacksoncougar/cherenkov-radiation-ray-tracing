/*
 * GlossySpecularMicrofacet.cpp
 *
 *  Created on: Jan 27, 2020
 *      Author: jackson.wiebe1
 */

#include <Microfacet.h>
#include <cmath>
#include "Constants.h"

using std::acos;
using std::tan;
using std::tan;

constexpr float e = 2.718282f;

template<>
double GlossySpecularMicrofacet<DistributionFunc::Beckmann>::D(
		const Vector3D& n, const Vector3D& wh) const {

	const float a = acos(n * wh);
	const float power = pow(tan(a) / r, 2.0f);

	return pow(e, -power) / (pow(r, 2.0f) * pow(cos(a), 4.0f));

}

template<>
double GlossySpecularMicrofacet<DistributionFunc::TrowbridgeReitz>::D(
		const Vector3D& n, const Vector3D& wh) const {

	const float a_sq = pow(r, 4.0f);
	const float ndotwh_sq = pow(n * wh, 2.0f);

	return a_sq * invPI / pow((ndotwh_sq * (a_sq - 1.0f)) + 1.0f, 2.0f);
}
