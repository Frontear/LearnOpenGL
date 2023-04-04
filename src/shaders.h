#ifndef FRONTEAR_SHADERS
#define FRONTEAR_SHADERS

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <GL/glew.h>

namespace frontear {
    class shaders {
    public:
        GLuint program;
        std::vector<GLuint> shdrs;

    public:
        shaders() {
            this->program = glCreateProgram();
        }

        ~shaders() {
            for (const auto &s : shdrs) {
                glDeleteShader(s);
            }

            shdrs.clear();

            glDeleteProgram(program);
        }

        void attach(GLenum type, const char* filename) {
            assert(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER), "Invalid shader type specified";

            std::ifstream file(filename, std::ios::in);
            std::ostringstream data;
            file >> data.rdbuf();

            std::string str = data.str();
            const GLchar* content = str.c_str();

            GLuint shdr = glCreateShader(type);
            shdrs.push_back(shdr);

            glShaderSource(shdr, 1, &content, nullptr);
            glCompileShader(shdr);

            GLint status;
            GLchar report[1024];

            glGetShaderiv(shdr, GL_COMPILE_STATUS, &status);
            if (!status) {
                glGetShaderInfoLog(shdr, sizeof(report) / sizeof(GLchar), nullptr, report);
                std::cerr << "Error compiling shader (" << filename << "): \n" << report << std::endl;
            }

            glAttachShader(program, shdr);
        }

        void finalize() {
            glLinkProgram(program);

            GLint status;
            GLchar report[1024];

            glGetProgramiv(program, GL_LINK_STATUS, &status);
            if (!status) {
                glGetProgramInfoLog(program, sizeof(report) / sizeof(GLchar), nullptr, report);
                std::cerr << "Error linking shader program: \n" << report << std::endl;
            }
        }

        GLint uniformLoc(const char* name) {
            return glGetUniformLocation(program, name);
        }

        void use() {
            glUseProgram(program);
        }
    };
};

#endif
