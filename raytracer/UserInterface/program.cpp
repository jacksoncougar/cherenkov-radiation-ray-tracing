//
// Created by root on 10/3/20.
//

#include "program.hpp"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"
#include "renderThread.h"
#include "stb_image_write.h"
#include <World.h>
#include <array>
#include <conio.h>
#include <fstream>
#include <glad/glad.h>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdio.h>
#include <thread>

struct shader_base {
    unsigned int id;
};

template<int E>
struct shader : public shader_base {
    explicit shader(const string &filename) {
        ifstream file;
        file.open(filename, ios::in); // opens as ASCII!
        assert(file.is_open());
        vector<char> source{istreambuf_iterator<char>(file), istreambuf_iterator<char>{}};

        id = glCreateShader(E);
        array<GLchar *, 1> source_lines = {source.data()};
        array<int, 1> source_sizes = {static_cast<int>(source.size())};

        glShaderSource(id, 1, source_lines.data(), source_sizes.data());
        glCompileShader(id);

        GLint compiled;
        glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);

        if (!compiled) {
            GLint blen = 0;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &blen);
            if (blen > 1) {
                vector<char> log(blen);
                glGetShaderInfoLog(id, blen, nullptr, log.data());
                cerr << "\t[glsl]\t" << string(log.data());
                exit(1);
            }
        }
        assert(compiled);
    }

    ~shader() {
        log("Deleting shader");
        glDeleteShader(id);
    }
};

struct program {
    unsigned int id;

    program(initializer_list<shader_base> shaders) {
        id = glCreateProgram();

        for (auto shader : shaders) {
            glAttachShader(id, shader.id);
        }
        glLinkProgram(id);
        {
            GLint linked;
            glGetProgramiv(id, GL_LINK_STATUS, &linked);

            if (!linked) {
                GLint blen = 0;
                GLsizei slen = 0;
                glGetProgramiv(id, GL_INFO_LOG_LENGTH, &blen);
                if (blen > 1) {
                    vector<char> log(blen);
                    glGetProgramInfoLog(id, blen, &slen, log.data());
                    cerr << "\t[glsl]\t" << string(log.data());
                    exit(1);
                }
            }
            assert(linked);
        }
    }

    ~program() {
        log("Deleting program");
        glDeleteProgram(id);
    }
};