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

#include "json.hpp"

#include "ui.h"

void log(std::string msg) {
    std::cout << "\t[log]\t" << msg;
}

template<typename...Args>
struct EventDelegate {
    std::vector<std::function<void(Args...)>> handlers;

    void operator()(Args... args) {
        for (const auto &handler: handlers) {
            handler(args...);
        }
    }

    void operator+=(std::function<void(Args...)> handler) {
        handlers.push_back(handler);
    }

};

struct program {

    EventDelegate<int> onKeyPressEvent{};

    GLFWwindow *window;
    unsigned int texture;
    unsigned int framebuffer;
    std::shared_ptr<RenderThread> renderThread;
    std::shared_ptr<World> world;
    std::shared_ptr<Instance> subject;
    std::shared_ptr<ImageTexture> image;
    std::shared_ptr<Material> active_material;
    std::shared_ptr<Grid> mesh;

    bool redraw = false;

    int width = 640;
    int height = 480;

    void onKeyPress(int key) {
        switch (key) {


            case GLFW_KEY_F1:
                load_silhouette_material();
                load_scene(); // creates world...

                renderThread->join(); // ignore currrent solution...
                renderThread = std::make_shared<RenderThread>(world);
                break;


            case GLFW_KEY_F2:
                load_highlights_material();
                load_scene(); // creates world...

                renderThread->join(); // ignore currrent solution...
                renderThread = std::make_shared<RenderThread>(world);
                break;


            case GLFW_KEY_F3:
                load_depth_material();
                load_scene(); // creates world...

                renderThread->join(); // ignore currrent solution...
                renderThread = std::make_shared<RenderThread>(world);
                break;

            case GLFW_KEY_X:
                subject->rotate_x(90.0f);
                break;
            case GLFW_KEY_Y:
                subject->rotate_y(90.0f);
                break;
            case GLFW_KEY_Z:
                subject->rotate_z(90.0f);
                break;
            case GLFW_KEY_UP:
                subject->scale(1.1f);
                break;
            case GLFW_KEY_DOWN:
                subject->scale(0.9f);
                break;
            default:
                onKeyPressEvent(key);
        }
        redraw = true;
    }

    void load_attribute_image(std::string filename) {
        image = std::make_shared<ImageTexture>(std::make_shared<Image>(filename));
    }

    void load_mesh_data(std::string filename) {
        mesh = std::make_shared<Grid>(new Mesh());
        mesh->read_smooth_triangles(const_cast<char *>(filename.c_str()));
        mesh->setup_cells();
    }

    void load_silhouette_material() {
        auto material = std::make_shared<svSilhouetteMaterial>();
        material->set_attribute_image(image);
        active_material = material;
        subject->set_material(active_material.get());

        onKeyPressEvent.handlers.clear();
        onKeyPressEvent += [material](int key) {
            std::cout << "onKeyPressEvent";
            if (key == GLFW_KEY_EQUAL) {
                material->r(material->r() + 1.0f);
            }
            if (key == GLFW_KEY_MINUS) {
                material->r(material->r() - 1.0f);
            }
        };
    }

    void load_highlights_material() {
        auto material = std::make_shared<svHighlightsMaterial>();
        material->set_attribute_image(image);
        active_material = material;
        subject->set_material(active_material.get());

        onKeyPressEvent.handlers.clear();
        onKeyPressEvent += [material](int key) {
            std::cout << "onKeyPressEvent";
            if (key == GLFW_KEY_EQUAL) {
                material->s(material->s() + 1.0f);
            }
            if (key == GLFW_KEY_MINUS) {
                material->s(material->s() - 1.0f);
            }
        };
    }

    void load_depth_material() {
        auto material = std::make_shared<svDepthMaterial>();
        material->set_attribute_image(image);
        active_material = material;
        subject->set_material(active_material.get());

        onKeyPressEvent.handlers.clear();
        onKeyPressEvent += [material](int key) {
            if (key == GLFW_KEY_EQUAL) {
                material->r(material->r() + 5.0f);
            }
            if (key == GLFW_KEY_MINUS) {
                material->r(material->r() - 5.0f);
            }
            if (key == GLFW_KEY_PAGE_UP) {
                material->z_min(material->z_min() + 5.0);
            }
            if (key == GLFW_KEY_PAGE_DOWN) {
                material->z_min(material->z_min() - 5.0);
            }
        };
    }

    void load_scene() {
    }

    void focus_subject() const {
        subject->compute_bounding_box();
        auto bbox = subject->get_bounding_box();
        double dx = (bbox.x1 - bbox.x0) / 2.0;
        double dy = (bbox.y1 - bbox.y0) / 2.0;
        double dz = (bbox.z1 - bbox.z0) / 2.0;

        world->camera_ptr->set_eye(bbox.x0 + dx, bbox.y0 + dy, bbox.z0 + 5 * dz);
        world->camera_ptr->set_lookat(bbox.x0 + dx, bbox.y0 + dy, bbox.z0 + dz);
        world->camera_ptr->compute_uvw();
    }

    program(const char *attribute_image_filename, const char *mesh_filename) {

        if (!glfwInit()) {
            std::cerr << "GLFW Initialization failed";
            exit(1);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        window = glfwCreateWindow(width, height, "", NULL, NULL);
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

        load_attribute_image(attribute_image_filename);
        load_mesh_data(mesh_filename);
        subject = std::make_shared<Instance>(mesh.get());;
        load_silhouette_material();

        world->objects.clear();
        world->vp.hres = width;
        world->vp.vres = height;
        world->add_object(subject.get());

        focus_subject();
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
        if (action == GLFW_PRESS) program->onKeyPress(key);
    }

    void update(float time = 0) {
        if (redraw) {
            glfwGetFramebufferSize(window, &width, &height);
            world->vp.hres = width;
            world->vp.vres = height;
            renderThread->join(); // ignore currrent solution...
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
