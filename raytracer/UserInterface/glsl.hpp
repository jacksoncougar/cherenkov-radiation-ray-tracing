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

#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "ui.h"

void log(std::string msg) {
    std::cout << "\t[log]\t" << msg;
}

namespace glsl {

}
struct program {
    GLFWwindow *window;
    unsigned int texture;
    unsigned int framebuffer;
    std::shared_ptr<program> null_program;
    std::shared_ptr<RenderThread> renderThread;

    program() {


        if (!glfwInit()) {
            std::cerr << "GLFW Initialization failed";
            exit(1);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        window = glfwCreateWindow(512, 512, "", NULL, NULL);
        if (!window) {
            std::cerr << "Window or OpenGL context creation failed";
            exit(1);
        }
        glfwMakeContextCurrent(window);
        gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
        glfwSwapInterval(1);

        // render given scene
        auto world = std::make_shared<World>();
        world->build();

        renderThread = std::make_shared<RenderThread>(world);
        world->paintArea = renderThread.get();
        world->camera_ptr->render_scene(*world);

        // setup render target & texture

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB8, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
        );

        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glFramebufferTexture2D(
            GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0
        );
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);


        while (!glfwWindowShouldClose(window)) {
            update();
            render();
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    void update(float time = 0) {
        // copy our render pixels to the framebuffer (e.g. screen).
        auto pixels = renderThread->pixel_data();
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexSubImage2D(
            GL_TEXTURE_2D, 0, 0, 0, 512, 512, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

        glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
        glBlitFramebuffer(
            0, 0, 512, 512, 0, 0, 512, 512, GL_COLOR_BUFFER_BIT, GL_LINEAR
        );
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    }

    void render() {
        // ...
    }

    ~program() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

#endif //WXRAYTRACER_GLSL_HPP
