//
// Created by root on 9/3/20.
//
#ifndef WXRAYTRACER_UI_H
#define WXRAYTRACER_UI_H

#include "World.h"
#include <thread>
#include <memory>

struct RenderPixel {
    int x, y;
    int r, g, b;
};

class RenderThread : public std::thread {

    std::shared_ptr<World> world;
    std::vector<RenderPixel> pixels;
public:
    explicit RenderThread(std::shared_ptr<World> world) : world(move(world)) {}

    vector<unsigned char> pixel_data();

    virtual void setPixel(int x, int y, int red, int green, int blue);
};

#endif //WXRAYTRACER_UI_H
