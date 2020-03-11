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

private:
    std::shared_ptr<World> world;
    std::vector<RenderPixel> pixels;
    std::mutex pixel_mutex;
public:
    explicit RenderThread(std::shared_ptr<World> world) : std::thread(&RenderThread::work, this),
                                                          world(std::move(world)) {
    }

    void work() {
        this->pixels.clear();
        this->world->paintArea = this;
        this->world->camera_ptr->render_scene(*this->world);
    }

    virtual void setPixel(int x, int y, int red, int green, int blue);

    std::vector<unsigned char> pixel_data(bool flip_y);
};

#endif //WXRAYTRACER_UI_H
