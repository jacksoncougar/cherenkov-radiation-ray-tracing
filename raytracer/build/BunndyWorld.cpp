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
#include <SVAttributeBasedMapping.hpp>
#include <MarchingRayTracer.hpp>
#include "Image.h"
#include "ImageTexture.hpp"

void World::build(void) {
    int num_samples = 1;

    // view plane

    vp.set_hres(512);
    vp.set_vres(512);
    vp.set_pixel_size(1.0);
    vp.set_samples(num_samples);

    // the ambient light here is the same as the default set in the World
    // constructor, and can therefore be left out


    background_color = RGBColor(0, 0, 0); // default color - this can be left out

    tracer_ptr = new MarchingRayTracer(this);

    // camera

    Pinhole *pinhole_ptr = new Pinhole;
    pinhole_ptr->set_eye(0, 10, 500);
    pinhole_ptr->set_lookat(0.0);
    pinhole_ptr->set_view_distance(600.0);
    pinhole_ptr->compute_uvw();
    set_camera(pinhole_ptr);



}

