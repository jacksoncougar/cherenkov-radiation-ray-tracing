//
// Created by root on 9/3/20.
//
#ifndef WXRAYTRACER_UI_H
#define WXRAYTRACER_UI_H

#include "World.h"
#include <thread>
#include <memory>
#include <mutex>

struct RenderPixel {
    int x, y;
    int r, g, b;
};

class RenderThread : public std::thread {
public:
    int width;
    int height;
private:
    std::shared_ptr<World> world;
    std::vector<RenderPixel> pixels;
    std::mutex pixel_mutex;
    std::mutex world_mutex;
public:
    explicit RenderThread(std::shared_ptr<World> world) : std::thread(&RenderThread::work, this),
                                                          world(std::move(world)) {
        width = this->world->vp.hres;
        height = this->world->vp.vres;
        this->world->paintArea = this;
        this->pixels.clear();
        this->world->camera_ptr->render_scene(*this->world);
    }

    void work() {
        //auto lock = std::lock_guard(world_mutex);
    }

    virtual void setPixel(int x, int y, int red, int green, int blue);

    std::vector<unsigned char> pixel_data(bool flip_y);
};

#endif //WXRAYTRACER_UI_H
