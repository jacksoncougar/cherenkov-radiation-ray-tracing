//
// Created by root on 9/3/20.
//

#include <World.h>
#include <memory>
#include <thread>
#include "ui.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image_write.h"

int main(int argc, char *argv[]) {
    // render given scene
    auto world = std::make_shared<World>();
    world->build();

    auto renderThread = std::make_shared<RenderThread>(world);
    world->paintArea = renderThread.get();

    world->camera_ptr->render_scene(*world);

    // save scene into image file

    auto pixels = renderThread->pixel_data();
    stbi_write_png("a.png", world->vp.hres, world->vp.vres, 3, pixels.data(), world->vp.hres * 3);
    exit(0);
}