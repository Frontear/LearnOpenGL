#ifndef FRONTEAR_SHADER
#define FRONTEAR_SHADER

#include <iostream>
#include <GL/glew.h>

namespace frontear {
    struct shader {
        int id;

        void create(GLenum type, const char* content) {
            id = glCreateShader(type);
            glShaderSource(id, 1, &content, nullptr); // usually expects an array of strings, hence the length parameter. however, we pass the entire string object, so we set to nullptr. same reason why count is 1.
            glCompileShader(id);

            int success;
            char log[1024];
            glGetShaderiv(id, GL_COMPILE_STATUS, &success);

            if (success != GL_TRUE) {
                glGetShaderInfoLog(id, sizeof(log) / sizeof(char), nullptr, log); // we dont need the length of how long the log is, hence why we set as nullptr
                std::cerr << "Error compiling shader:\n" << log << std::endl;
            }
        }

        void dispose() {
            glDeleteShader(id); // disposal is usually called after program is linked. however, shader itself does not actually get deleted until it detaches from the program, which probably happens alongside glfw terminate or something to that effect.
        }
    };
}

#endif
