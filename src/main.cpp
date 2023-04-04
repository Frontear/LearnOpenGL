#define GLFW_INCLUDE_NONE // Disable inclusion of GL headers, we use GLEW

#include <cstddef>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "buffer.h"
#include "shaders.h"

int main() {
    glfwSetErrorCallback([](int err, const char* msg) {
        std::cerr << "GLFW Error (code " << err << "): " << msg << std::endl;
    });

    if (glfwInit()) {
        std::cout << "Running GLFW " << glfwGetVersionString() << std::endl;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        GLFWwindow* window = glfwCreateWindow(854, 480, "LearnOpenGL", nullptr, nullptr);
        if (window != nullptr) {
            glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int, int action, int) {
                if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                    glfwSetWindowShouldClose(window, true);
                }
            });

            glfwMakeContextCurrent(window);

            glfwSetFramebufferSizeCallback(glfwGetCurrentContext(), [](GLFWwindow*, GLsizei width, GLsizei height) {
                glViewport(0, 0, width, height);
            });

            glewExperimental = true;
            if (glewInit() == GLEW_OK) {
                GLsizei width, height;
                glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
                //glViewport(0, 0, width, height);

                // -- SHADER
                /*
                std::string vert_shader_str = read_file("assets/default.vert");
                std::string frag_shader_str = read_file("assets/default.frag");

                const GLchar* vert_shader_data = vert_shader_str.c_str();
                const GLchar* frag_shader_data = frag_shader_str.c_str();

                GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
                GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);

                glShaderSource(vert_shader, 1, &vert_shader_data, nullptr);
                glShaderSource(frag_shader, 1, &frag_shader_data, nullptr);

                glCompileShader(vert_shader);
                glCompileShader(frag_shader);

                GLuint shader_program = glCreateProgram();
                glAttachShader(shader_program, vert_shader.ID);
                glAttachShader(shader_program, frag_shader.ID);
                glLinkProgram(shader_program);
                */

                frontear::shaders shaders;
                shaders.attach(GL_VERTEX_SHADER, "assets/default.vert");
                shaders.attach(GL_FRAGMENT_SHADER, "assets/default.frag");
                shaders.finalize();

                // -- END SHADER
                // -- VAO VBO EBO

                GLuint VAO[1];
                glGenVertexArrays(sizeof(VAO) / sizeof(GLuint), VAO);
                glBindVertexArray(VAO[0]);

                GLfloat w = 400 / (float) width;
                GLfloat h = 400 / (float) height;

                struct vert_buff {
                    GLfloat xy[2]; // location 0
                    GLfloat rgb[3]; // location 1
                };

                // counter clockwise
                vert_buff buffer_data[] = {
                    { { -w, +h },       { 255 / 255.0f, 202 / 255.0f, 217 / 255.0f } },
                    { { -w, -h },       { 177 / 255.0f, 155 / 255.0f, 217 / 255.0f } },
                    { { +w, -h },       { 183 / 255.0f, 209 / 255.0f, 226 / 255.0f } },
                    { { +w, +h },       { 100 / 255.0f, 224 / 255.0f, 100 / 255.0f } },
                };

                //GLuint VBO[1];
                //glGenBuffers(sizeof(VBO) / sizeof(GLuint), VBO);
                //glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
                //glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data), buffer_data, GL_STATIC_DRAW);

                frontear::buffer VBO(GL_ARRAY_BUFFER);
                VBO.upload(buffer_data, GL_STATIC_DRAW);

                // counter clockwise
                GLuint index_data[] = {
                    0, 1, 2,
                    0, 2, 3
                };

                //GLuint EBO[1];
                //glGenBuffers(sizeof(EBO) / sizeof(GLuint), EBO);
                //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
                //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);

                frontear::buffer EBO(GL_ELEMENT_ARRAY_BUFFER);
                EBO.upload(index_data, GL_STATIC_DRAW);

                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(buffer_data) / 4, (void*) offsetof(vert_buff, xy));
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(buffer_data) / 4, (void*) offsetof(vert_buff, rgb));

                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);

                glBindVertexArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

                // -- END VAO VBO EBO

                GLint matrix_loc = shaders.uniformLoc("matrix");

                glfwSwapInterval(1); // vsync
                glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
                while (!glfwWindowShouldClose(window)) {
                    glClear(GL_COLOR_BUFFER_BIT);

                    shaders.use();

                    //glm::mat4 identity(1.0f);
                    //identity = glm::rotate(identity, (float) glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

                    //glUniformMatrix4fv(matrix_loc, 1, false, glm::value_ptr(identity)); // the rotation is FUCKED LMAO

                    glBindVertexArray(VAO[0]);
                    glDrawElements(GL_TRIANGLES, sizeof(index_data) / sizeof(GLuint), GL_UNSIGNED_INT, nullptr);

                    glfwSwapBuffers(window);
                    glfwPollEvents();
                }

                //glDeleteShader(vert_shader);
                //glDeleteShader(frag_shader);
                //glDeleteProgram(shader_program);

                glDeleteVertexArrays(sizeof(VAO) / sizeof(GLuint), VAO);
                //glDeleteBuffers(sizeof(VBO) / sizeof(GLuint), VBO);
                //glDeleteBuffers(sizeof(EBO) / sizeof(GLuint), EBO);
            }

            glfwDestroyWindow(window);
        }

        glfwTerminate();

        return 0;
    }

    return -1;
}
