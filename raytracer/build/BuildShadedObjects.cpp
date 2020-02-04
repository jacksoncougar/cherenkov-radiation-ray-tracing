// This builds a scene that consists of 35 shaded spheres and a plane.
// The objects are illuminated by a directional light and rendered with
// ambient and diffuse shading.
// Perspective viewing is used with a pinhole camera.
// Jittered sampling for antialiasing is hardwired into the PinHole::render_scene function.
// There are no sampler classes in this project.
// These are in the Chapter 5 download file.
// The spheres are the same as those in the Chapter 14 page one image. 


void World::build(void) {
	int num_samples = 1;

	// view plane  

	vp.set_hres(400);
	vp.set_vres(400);
	vp.set_pixel_size(0.5);
	vp.set_samples(num_samples);

	// the ambient light here is the same as the default set in the World
	// constructor, and can therefore be left out

	Ambient* ambient_ptr = new Ambient;
	ambient_ptr->scale_radiance(1.0);
	set_ambient_light(ambient_ptr);

	background_color = black;			// default color - this can be left out

	tracer_ptr = new RayCast(this);

	// camera

	Pinhole* pinhole_ptr = new Pinhole;
	pinhole_ptr->set_eye(0, 0, 500);
	pinhole_ptr->set_lookat(0.0);
	pinhole_ptr->set_view_distance(600.0);
	pinhole_ptr->compute_uvw();
	set_camera(pinhole_ptr);

	// light

	Directional* light_ptr1 = new Directional;
	light_ptr1->set_direction(100, 100, 200);
	light_ptr1->scale_radiance(3.0);
	add_light(light_ptr1);

	// colors

	RGBColor yellow(1, 1, 0);										// yellow
	RGBColor brown(0.71, 0.40, 0.16);								// brown
	RGBColor darkGreen(0.0, 0.41, 0.41);							// darkGreen
	RGBColor orange(1, 0.75, 0);									// orange
	RGBColor green(0, 0.6, 0.3);									// green
	RGBColor lightGreen(0.65, 1, 0.30);							// light green
	RGBColor darkYellow(0.61, 0.61, 0);							// dark yellow
	RGBColor lightPurple(0.65, 0.3, 1);							// light purple
	RGBColor darkPurple(0.5, 0, 1);								// dark purple
	RGBColor grey(0.25);											// grey

	// Matte material reflection coefficients - common to all materials

	float ka = 0.25;
	float kd = 0.75;

	// spheres

	Microfacet* matte_ptr1 = new Microfacet;
	matte_ptr1->set_ka(ka);
	matte_ptr1->set_kd(kd);
	matte_ptr1->set_cd(grey);
	Sphere* sphere_ptr1 = new Sphere(Point3D(5, 3, 0), 30);
	sphere_ptr1->set_material(matte_ptr1);	   						// yellow
	add_object(sphere_ptr1);

	// vertical plane

	Matte* matte_ptr36 = new Matte;
	matte_ptr36->set_ka(ka);
	matte_ptr36->set_kd(kd);
	matte_ptr36->set_cd(grey);
	Plane* plane_ptr = new Plane(Point3D(0, 0, -150), Normal(0, 0, 1));
	plane_ptr->set_material(matte_ptr36);
	add_object(plane_ptr);
}

