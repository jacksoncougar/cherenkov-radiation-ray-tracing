// This builds a scene that consists of 35 shaded spheres and a plane.
// The objects are illuminated by a directional light and rendered with
// ambient and diffuse shading.
// Perspective viewing is used with a pinhole camera.
// Jittered sampling for antialiasing is hardwired into the PinHole::render_scene function.
// There are no sampler classes in this project.
// These are in the Chapter 5 download file.
// The spheres are the same as those in the Chapter 14 page one image. 

#include <World.h>
#include <Directional.h>
#include <Pinhole.h>
#include <RayCast.h>
#include <Microfacet.h>
#include <Matte.h>
#include <Plane.h>
#include <GlossySpecular.h>
#include <Instance.h>
#include "Image.h"
#include "ImageTexture.h"

void World::build(void) {
	int num_samples = 1;

	// view plane

	vp.set_hres(400);
	vp.set_vres(400);
	vp.set_pixel_size(0.05);
	vp.set_samples(num_samples);

	// the ambient light here is the same as the default set in the World
	// constructor, and can therefore be left out

	Ambient *ambient_ptr = new Ambient;
	ambient_ptr->scale_radiance(1.0);
	set_ambient_light(ambient_ptr);

	background_color = RGBColor(1, 1, 0); // default color - this can be left out

	tracer_ptr = new RayCast(this);

	// camera

	Pinhole *pinhole_ptr = new Pinhole;
	pinhole_ptr->set_eye(0, 0, 500);
	pinhole_ptr->set_lookat(0.0);
	pinhole_ptr->set_view_distance(600.0);
	pinhole_ptr->compute_uvw();
	set_camera(pinhole_ptr);

	// light

	Directional *light_ptr1 = new Directional;
	light_ptr1->set_direction(-20, 20, 30);
	light_ptr1->scale_radiance(0.80);
	add_light(light_ptr1);

	Directional *light_ptr2 = new Directional;
	light_ptr1->set_direction(0, 20, 0);
	light_ptr1->scale_radiance(0.80);
	add_light(light_ptr2);

	Image test("assets/ppm/fig-10b.ppm");
	ImageTexture texture;

	// colors

	RGBColor yellow(1, 1, 0);                                        // yellow
	RGBColor brown(0.71, 0.40, 0.16);                                // brown
	RGBColor darkGreen(0.0, 0.41, 0.41);                            // darkGreen
	RGBColor orange(1, 0.75, 0);                                    // orange
	RGBColor green(0, 0.6, 0.3);                                    // green
	RGBColor lightGreen(0.65, 1, 0.30);                           // light green
	RGBColor darkYellow(0.61, 0.61, 0);                           // dark yellow
	RGBColor lightPurple(0.65, 0.3, 1);                          // light purple
	RGBColor darkPurple(0.5, 0, 1);                               // dark purple
	RGBColor grey(0.25);                                            // grey

	// Matte material reflection coefficients - common to all materials

	float ka = 0.3;
	float kd = 0.8;

	auto gold = RGBColor { 1.00f, 0.71f, 0.29f };
	auto silver = RGBColor { 0.95, 0.93, 0.88 };
	auto copper = RGBColor { 0.95, 0.64, 0.54 };
	auto iron = RGBColor { 0.56, 0.57, 0.58 };
	auto aluminum = RGBColor { 0.91, 0.92, 0.92 };

	auto color = aluminum;

	auto *material = new Microfacet;
	material->set_ka(ka);
	material->set_kd(kd);
	material->set_cd(color);

	material->set_roughness(0.5f);
	material->set_specular_colour(color);

	// bunny

	auto attributeBasedMaterial = std::make_shared<svAttributeBasedMapping>();

	Grid *bunny = new Grid(new Mesh);
	bunny->read_smooth_triangles("assets/ply/Venus.ply")
	bunny->setup_cells();

	Instance *instance = new Instance(bunny);
	instance->scale(70, 70, 70);
	instance->translate(0, -5, 0);
	instance->rotate_y(35.0f);
	instance->rotate_x(15.0f);
	instance->set_material(attributeBasedMaterial);
	instance->compute_bounding_box();

	add_object(instance);

	// spheres

	Sphere *sphere_ptr1 = new Sphere(Point3D(5, 3, 0), 60);
	sphere_ptr1->set_material(material);
	//add_object(sphere_ptr1);

	// vertical plane

	Matte *matte_ptr36 = new Matte;
	matte_ptr36->set_ka(ka);
	matte_ptr36->set_kd(kd);
	matte_ptr36->set_cd(RGBColor(1));
	Plane *plane_ptr = new Plane(Point3D(0, 0, -150), Normal(0, 0, 1));
	plane_ptr->set_material(matte_ptr36);
	add_object(plane_ptr);

}

