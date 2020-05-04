// This file contains the definition of the Pinhole class

#include "Constants.h" 
#include "Point3D.h"
#include "Vector3D.h"
#include "Pinhole.h"
#include <math.h>
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <future>
#include <vector>

// ----------------------------------------------------------------------------- default constructor

Pinhole::Pinhole(void)
	: Camera(),
	d(500),
	zoom(1.0)
{}


// ----------------------------------------------------------------------------- copy constructor

Pinhole::Pinhole(const Pinhole& c)
	: Camera(c),
	d(c.d),
	zoom(c.zoom)
{}


// ----------------------------------------------------------------------------- clone

Camera*
Pinhole::clone(void) const {
	return (new Pinhole(*this));
}



// ----------------------------------------------------------------------------- assignment operator

Pinhole&
Pinhole::operator= (const Pinhole& rhs) {
	if (this == &rhs)
		return (*this);

	Camera::operator= (rhs);

	d = rhs.d;
	zoom = rhs.zoom;

	return (*this);
}


// ----------------------------------------------------------------------------- destructor

Pinhole::~Pinhole(void) {}


// ----------------------------------------------------------------------------- get_direction

Vector3D
Pinhole::get_direction(const Point2D& p) const {
	Vector3D dir = p.x * u + p.y * v - d * w;
	dir.normalize();

	return(dir);
}



// ----------------------------------------------------------------------------- render_scene
auto spiral = [&](int width, int height, auto function) {
	int x = 0;
	int y = 0;
	int dx = 0;
	int dy = -1;

	int range = std::pow(std::max(width, height), 2);
	for (int i = 0; i < range; ++i)
	{
		if ((-width / 2 < x && x <= width / 2) && (-height / 2 < y && y <= height / 2)) {

			auto wx = (width / 2 + x) - 1;
			auto wy = (height / 2 + y) - 1;

			function(wx, wy);

		}
		if (x == y || (x < 0 && x == -y) || (x > 0 && x == 1 - y)) {
			auto [ty, tx] = std::make_tuple(-dy, dx);
			dx = ty;
			dy = tx;
		}
		x = x + dx;
		y = y + dy;
	}
};

auto spiral_async = [&](int width, int height, auto function) {
	int x = 0;
	int y = 0;
	int dx = 0;
	int dy = -1;

	int range = std::pow(std::max(width, height), 2);
	std::vector<std::future<void>> tasks;
	for (int i = 0; i < range; ++i)
	{
		if ((-width / 2 < x && x <= width / 2) && (-height / 2 < y && y <= height / 2)) {

			auto wx = (width / 2 + x) - 1;
			auto wy = (height / 2 + y) - 1;

			tasks.emplace_back(std::async(std::launch::async | std::launch::deferred, function, wx, wy));

		}
		if (x == y || (x < 0 && x == -y) || (x > 0 && x == 1 - y)) {
			auto [ty, tx] = std::make_tuple(-dy, dx);
			dx = ty;
			dy = tx;
		}
		x = x + dx;
		y = y + dy;
	}
	for (auto&& task : tasks) {
		task.get();
	}

};

void
Pinhole::render_scene(const World& w) {
	RGBColor	L;
	ViewPlane	vp(w.vp);
	int 		depth = 0;
	int n = (int)sqrt((float)vp.num_samples);

	vp.s /= zoom;

	int X = vp.hres;
	int Y = vp.vres;

	// ref: https://stackoverflow.com/questions/398299/looping-in-a-spiral/1555236

	std::mutex world_mutex;

	int tilesize = 40;
	int tile_X = X / tilesize;
	int tile_Y = Y / tilesize;
	bool abort = false;


	//#pragma omp parallel for
	spiral_async(tile_X, tile_Y, [&](int tx, int ty) {
		spiral(tilesize, tilesize, [&](int x, int y) {
			if (abort || !CANCEL_THREAD.test_and_set()) {
				abort = true;
				return;
			}
			RGBColor L = black;
			Ray	ray;
			int wx, wy;
			{
				wx = tx * tilesize + (x);
				wy = ty * tilesize + (y);

				Point2D pp;		// sample point on a pixel
				pp.x = vp.s * (wx - 0.5 * X);
				pp.y = vp.s * (wy - 0.5 * Y);
				ray.o = eye;
				ray.d = get_direction(pp);
			}
			L += w.tracer_ptr->trace_ray(ray, depth);

			L *= exposure_time;

			if (0 <= wx && wx < vp.hres && 0 <= wy && wy < vp.vres) {
				w.display_pixel(wy, wx, L);
			}

			});
		});


}


