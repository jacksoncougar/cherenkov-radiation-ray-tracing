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
#include <fstream>
#include <filesystem>

#include "json.hpp"

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
    std::shared_ptr<Mesh> mesh;
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

        world = std::make_shared<World>();
        object = std::make_shared<Instance>();
        object->translate(0, -100, 0);
        object->scale(2, 2, 2);
        load_scene("assets/ppm/fig-10b.ppm", "assets/ply/Venus-Low.ply");

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

    void load_scene(std::string image_filename, std::string mesh_filename) {

        world->build();
        world->vp.hres = width;
        world->vp.vres = height;

        std::cout << "CWD: " << std::filesystem::current_path();

        image = std::make_shared<Image>(image_filename);
        material = std::make_shared<svAttributeBasedMapping>(std::make_shared<ImageTexture>(image));

        Grid *bunny = new Grid(new Mesh);
        
        bunny->read_smooth_triangles(const_cast<char*>(mesh_filename.c_str()));
        bunny->setup_cells();


        object->set_object(bunny);
        object->set_material(material.get());
        object->compute_bounding_box();

        world->add_object(object.get());

        if(renderThread)
            renderThread->join();
        renderThread = std::make_shared<RenderThread>(world);
    }


    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        auto program = reinterpret_cast<struct program *>(glfwGetWindowUserPointer(window));
        if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
            program->object->rotate_y(15.0f);
            program->redraw = true;
        }
        if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
            program->object->rotate_y(-15.0f);
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
        if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
            std::ifstream ifs("assets/settings.json");
            nlohmann::json settings;
            ifs >> settings;
            ifs.close();
            float value = settings["r"];
            std::string image_filename = settings["image"];
            std::cout << "Updating image: " << image_filename << "\n";

            program->load_scene(image_filename, "assets/ply/Venus-Low.ply");

            // program->image = std::make_shared<Image>(image_filename);
            // program->material = std::make_shared<svAttributeBasedMapping>(
            //         std::make_shared<ImageTexture>(program->image));
            // std::cout << "r: " << value << "\n";
            // program->material->r(value);
            // program->object->set_material(program->material.get());
            program->redraw = true;
        }
    }

    void update(float time = 0) {

        if (redraw) {
            glfwGetFramebufferSize(window, &width, &height);
            world->vp.hres = width;
            world->vp.vres = height;
            if(renderThread->joinable())
                renderThread->join();
            renderThread = std::make_shared<RenderThread>(world);

            redraw = false;
        }
    }

    void render() {
        // copy our render pixels to the framebuffer (e.g. screen).
        if (!renderThread) return;
        auto pixels = renderThread->pixel_data(false);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, renderThread->width, renderThread->height, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
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
