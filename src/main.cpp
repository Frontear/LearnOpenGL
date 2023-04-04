#define GLFW_INCLUDE_NONE // Disable inclusion of GL headers, we use GLEW

#include <cstddef>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shaders.h"
#include "vert_array.h"

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

                frontear::shaders shaders;
                shaders.attach(GL_VERTEX_SHADER, "assets/default.vert");
                shaders.attach(GL_FRAGMENT_SHADER, "assets/default.frag");
                shaders.finalize();

                // -- END SHADER
                // -- VAO VBO EBO

                frontear::vert_array VAO;
                VAO.use();

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

                VAO.buffer(GL_ARRAY_BUFFER, buffer_data, GL_STATIC_DRAW);

                // counter clockwise
                GLuint index_data[] = {
                    0, 1, 2,
                    0, 2, 3
                };

                VAO.buffer(GL_ELEMENT_ARRAY_BUFFER, index_data, GL_STATIC_DRAW);

                VAO.attrib(0, 2, sizeof(buffer_data) / 4, (void*) offsetof(vert_buff, xy));
                VAO.attrib(1, 3, sizeof(buffer_data) / 4, (void*) offsetof(vert_buff, rgb));

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

                    VAO.use();
                    glDrawElements(GL_TRIANGLES, sizeof(index_data) / sizeof(GLuint), GL_UNSIGNED_INT, nullptr);

                    glfwSwapBuffers(window);
                    glfwPollEvents();
                }
            }

            glfwDestroyWindow(window);
        }

        glfwTerminate();

        return 0;
    }

    return -1;
}
