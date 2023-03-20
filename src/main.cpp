#ifdef _WIN32
#define GLFW_DLL
#endif

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#include "shader.h"
#include "program.h"

#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main(void) {
    if (glfwInit() == GLFW_TRUE) {
        // is OpenGL 4.6 too new?
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #ifdef __APPLE__ && __MACH__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
        #endif

        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE); // for fun

        auto window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "LearnOpenGL", nullptr, nullptr);
        if (window != nullptr) {
            glfwMakeContextCurrent(window);

            if (glewInit() == GLEW_OK) {
                glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT); // sometimes this is set by default when setting the window context. its implementation specific, aka unreliable to expect
                glfwSetFramebufferSizeCallback(window, [](auto window, int width, int height) {
                    glViewport(0, 0, width, height);
                });

                frontear::shader vertex_shader;
                vertex_shader.create(GL_VERTEX_SHADER,
                R"glsl(
                #version 460 core

                layout (location = 0) in vec2 xy;
                layout (location = 1) in vec3 rgb;

                uniform mat4 matrix = mat4(1.0); // default value of identity matrix

                out vec4 FragColor;

                void main() {
                    gl_Position = matrix * vec4(xy, 1.0, 1.0);
                    FragColor = vec4(rgb, 1.0);
                }
                )glsl");

                frontear::shader fragment_shader;
                fragment_shader.create(GL_FRAGMENT_SHADER,
                R"glsl(
                #version 460 core

                in vec4 FragColor;
                out vec4 diffuseColor; // apparently in GL 4.6 there's no gl_FragColor? idk lol

                void main() {
                    diffuseColor = FragColor;
                }
                )glsl");

                frontear::program shader_program;
                shader_program.attach(vertex_shader);
                shader_program.attach(fragment_shader);
                shader_program.finalize();

                GLuint VAO;
                glGenVertexArrays(1, &VAO);
                glBindVertexArray(VAO); // stores binded buffers, attrib data

                int width, height;
                glfwGetWindowSize(window, &width, &height);

                float buffer[] = {
                    // coordinates                                              // colors
                    -((width / 2.0f) / width),  +((height / 2.0f) / height),    255 / 255.0, 202 / 255.0, 217 / 255.0, // top left
                    +((width / 2.0f) / width),  +((height / 2.0f) / height),    177 / 255.0, 233 / 255.0, 254 / 255.0, // top right
                    -((width / 2.0f) / width),  -((height / 2.0f) / height),    255 / 255.0, 202 / 255.0, 217 / 255.0, // bottom left
                    +((width / 2.0f) / width),  -((height / 2.0f) / height),    177 / 255.0, 233 / 255.0, 254 / 255.0, // bottom right
                };

                unsigned int index[] = {
                    0, 1, 2,
                    1, 2, 3
                };

                GLuint BO[1];
                glGenBuffers(sizeof(BO) / sizeof(GLuint), BO);

                glBindBuffer(GL_ARRAY_BUFFER, BO[0]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(buffer), buffer, GL_STATIC_DRAW);

                //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BO[1]);
                //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(buffer) / 4, (void*) 0);
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(buffer) / 4, (void*) (2 * sizeof(float)));
                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);

                // no need to unbind but usually unbinding the VAO and BO's would go here.

                auto radians = 0.0f, x = 0.0f, y = 0.0f;
                while (glfwWindowShouldClose(window) != GLFW_TRUE) {
                    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                        glfwSetWindowShouldClose(window, GLFW_TRUE);
                    }

                    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
                        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                            y += 0.05;
                        }
                        else {
                            y += 0.01;
                        }
                    }
                    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                            y -= 0.05;
                        }
                        else {
                            y -= 0.01;
                        }
                    }
                    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                            radians += 0.01;
                        }
                        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                            x -= 0.05;
                        }
                        else {
                            x -= 0.01;
                        }
                    }
                    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                            radians -= 0.01;
                        }
                        else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                            x += 0.05;
                        }
                        else {
                            x += 0.01;
                        }
                    }

                    glClearColor(0.15, 0.15, 0.15, 0.3);
                    glClear(GL_COLOR_BUFFER_BIT);

                    shader_program.use();

                    glm::mat4 matrix(1.0f);
                    matrix = glm::translate(matrix, glm::vec3(x, y, 0.0));
                    matrix = glm::rotate(matrix, radians, glm::vec3(0.0, 0.0, 1.0)); // rotate on the z-axis

                    glUniformMatrix4fv(glGetUniformLocation(shader_program.id, "matrix"), 1, GL_FALSE, glm::value_ptr(matrix));

                    glBindVertexArray(VAO);
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, index); // if index is set to nullptr, will use EBO buffer data, otherwise can pull directly from here without allocating an EBO.

                    glfwSwapBuffers(window);
                    glfwPollEvents();
                }
            }
        }

        glfwTerminate();

        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
