#ifndef __POINTLIGHT__
#define __POINTLIGHT__

#include "Vector3D.h"
#include "RGBColor.h"
#include "Ray.h"
#include "Light.h"

#include <cmath>

class ShadeRec;


class ParticleEmitter : public Light {
public:

	Vector3D position;
	RGBColor color;
	float intensity = 1.0f;
	float falloff = 2.0f;

	ParticleEmitter(Vector3D position, RGBColor color) : position(position), color(color) {}

	virtual ParticleEmitter* clone(void) const override { return nullptr; }
	virtual	~ParticleEmitter(void) {}

	virtual Vector3D get_direction(ShadeRec& sr) { return  (position - sr.hit_point).hat(); }

	void set_color(RGBColor color) {
		this->color = color;
	}

	void set_intensity(float intensity) {
		this->intensity = intensity;
	}

	void set_falloff(float falloff) {
		this->falloff = falloff;
	}

	virtual RGBColor L(ShadeRec& sr) {
		auto w_bar = Vector3D(sr.hit_point - position).hat();
		auto distance_squared = Vector3D(sr.hit_point - position).len_squared();
		RGBColor result = intensity * color / std::pow(distance_squared,falloff);
		return result;
	}
};

#endif