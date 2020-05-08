//
// Created by root on 9/3/20.
//

#include "renderThread.h"
#include <World.h>

void RenderThread::setPixel(int x, int y, int red, int green, int blue) {
    auto lock = std::lock_guard(pixel_mutex);
    pixels.push_back(RenderPixel({x, y, red, green, blue}));
}

std::vector<unsigned char> RenderThread::pixel_data(bool flip_y) {

    int stride = 3;
    int width = world->vp.hres;
    int height = world->vp.vres;
    std::vector<unsigned char> buffer(width * height * stride);

    auto lock = std::lock_guard(pixel_mutex);
    for (auto pixel : pixels) {
        auto &[x, y, r, g, b] = pixel;
        //if (flip_y) y = height - y - 1;
        buffer[y * width * stride + x * stride + 0] = static_cast<unsigned char>(r);
        buffer[y * width * stride + x * stride + 1] = static_cast<unsigned char>(g);
        buffer[y * width * stride + x * stride + 2] = static_cast<unsigned char>(b);
    }
    return buffer;
}
