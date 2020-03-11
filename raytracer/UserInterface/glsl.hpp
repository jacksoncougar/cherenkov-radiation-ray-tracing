//
// Created by root on 10/3/20.
//

#ifndef WXRAYTRACER_GLSL_HPP
#define WXRAYTRACER_GLSL_HPP

#include <glad/glad.h>
#include "GLFW/glfw3.h"

#include <iostream>
#include <memory>
#include <string>
#include <functional>
#include <Instance.h>
#include <GlossySpecular.h>
#include <Image.h>
#include <ImageTexture.hpp>
#include <SVAttributeBasedMapping.hpp>

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "ui.h"

void log(std::string msg) {
    std::cout << "\t[log]\t" << msg;
}

struct program {
    GLFWwindow *window;
    unsigned int texture;
    unsigned int framebuffer;
    std::shared_ptr<RenderThread> renderThread;
    std::shared_ptr<World> world;
    std::shared_ptr<Instance> object;
    std::shared_ptr<Image> image;
    std::shared_ptr<svAttributeBasedMapping> material;

    bool redraw = false;

    int width = 640;
    int height = 480;

    program() {

        if (!glfwInit()) {
            std::cerr << "GLFW Initialization failed";
            exit(1);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        window = glfwCreateWindow(640, 480, "", NULL, NULL);
        if (!window) {
            std::cerr << "Window or OpenGL context creation failed";
            exit(1);
        }
        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, this);
        gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
        glfwSwapInterval(1);
        glfwSetKeyCallback(window, key_callback);

        glfwGetFramebufferSize(window, &width, &height);

        // setup render_scene

        world = std::make_shared<World>();
        world->build();
        world->vp.hres = width;
        world->vp.vres = height;

        image = std::make_shared<Image>("assets/ppm/fig-10b.ppm");
        material = std::make_shared<svAttributeBasedMapping>(std::make_shared<ImageTexture>(image));

        Grid *bunny = new Grid(new Mesh);
        bunny->read_smooth_triangles("assets/ply/Venus-Low.ply");
        bunny->setup_cells();

        object = std::make_shared<Instance>(bunny);
        object->translate(0, -100, 0);
        object->scale(2, 2, 2);
        object->set_material(material.get());
        object->compute_bounding_box();

        world->add_object(object.get());

        renderThread = std::make_shared<RenderThread>(world);

        // setup render target & texture

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     nullptr);

        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        while (!glfwWindowShouldClose(window)) {
            update();
            render();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        auto program = reinterpret_cast<struct program *>(glfwGetWindowUserPointer(window));
        if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
            program->object->rotate_y(5.0f);
            program->redraw = true;
        }
        if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
            program->object->rotate_y(-5.0f);
            program->redraw = true;
        }
        if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
            program->object->scale(1.1f);
            program->redraw = true;
        }
        if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
            program->object->scale(0.9f);
            program->redraw = true;
        }
    }

    void update(float time = 0) {

        if (redraw) {
            glfwGetFramebufferSize(window, &width, &height);
            world->vp.hres = width;
            world->vp.vres = height;
            renderThread->join();
            renderThread = std::make_shared<RenderThread>(world);
            //renderThread->join();
            redraw = false;
        }
    }

    void render() {
        // copy our render pixels to the framebuffer (e.g. screen).

        auto pixels = renderThread->pixel_data(false);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     pixels.data());
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glBlitFramebuffer(0, 0, width, height, 0, height, width, 0, GL_COLOR_BUFFER_BIT, GL_LINEAR);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    }

    ~program() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

#endif //WXRAYTRACER_GLSL_HPP
