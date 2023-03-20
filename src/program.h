#ifndef FRONTEAR_PROGRAM
#define FRONTEAR_PROGRAM

#include "shader.h"
#include <GL/glew.h>

namespace frontear {
    struct program {
        int id;

        program() {
            id = glCreateProgram();
        }

        void attach(shader shdr) {
            glAttachShader(id, shdr.id);

            shdr.dispose(); // wont actually delete the shader, just flags it for deletion.
        }

        void finalize() {
            glLinkProgram(id);

            int success;
            char log[1024];
            glGetProgramiv(id, GL_LINK_STATUS, &success);

            if (success != GL_TRUE) {
                glGetProgramInfoLog(id, sizeof(log) / sizeof(char), nullptr, log);
                std::cerr << "Error linking shader program:\n" << log << std::endl;
            }
        }

        void use() {
            glUseProgram(id);
        }
    };
}

#endif
