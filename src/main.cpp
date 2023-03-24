#define GLFW_INCLUDE_NONE // prevents GLFW from linking OpenGL headers, we use GLEW for this

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

int main() {
    glfwSetErrorCallback([](int error, const char* desc) {
        std::cerr << "GLFW Error: " << desc << std::endl;
    });

    if (glfwInit() == GLFW_TRUE) {
        // Use OpenGL version 4.6
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Only allow modern functions, none of the previous immediate mode legacy functions.

        GLFWwindow* window = glfwCreateWindow(800, 800, "LearnOpenGL", nullptr, nullptr);
        if (window != nullptr) {
            glfwMakeContextCurrent(window);

            if (glewInit() == GLEW_OK) {
                GLsizei width, height;
                glfwGetFramebufferSize(window, &width, &height);
                glViewport(0, 0, width, height);

                std::string vcontents;
                std::string fcontents;

                std::ifstream vertex("assets/default.vert");
                std::ifstream fragment("assets/default.frag");

                if (vertex) {
                    std::stringstream contents;
                    contents << vertex.rdbuf();

                    vertex.close();
                    vcontents = contents.str();
                }

                if (fragment) {
                    std::stringstream contents;
                    contents << fragment.rdbuf();

                    fragment.close();
                    fcontents = contents.str();
                }

                const GLchar* vshader = vcontents.c_str();
                const GLchar* fshader = fcontents.c_str();

                GLint status;
                GLchar report[1024];

                GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vertex_shader, 1, &vshader, nullptr);
                glCompileShader(vertex_shader);

                glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
                if (status != GL_TRUE) {
                    glGetShaderInfoLog(vertex_shader, sizeof(report) / sizeof(GLchar), nullptr, report);
                    std::cerr << "Error compiling vertex shader:\n" << report << std::endl;
                }

                GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fragment_shader, 1, &fshader, nullptr);
                glCompileShader(fragment_shader);

                glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
                if (status != GL_TRUE) {
                    glGetShaderInfoLog(fragment_shader, sizeof(report) / sizeof(GLchar), nullptr, report);
                    std::cerr << "Error compiling fragment shader:\n" << report << std::endl;
                }

                GLuint shader_program = glCreateProgram();
                glAttachShader(shader_program, vertex_shader);
                glAttachShader(shader_program, fragment_shader);
                glLinkProgram(shader_program);

                GLuint VAO;
                glGenVertexArrays(1, &VAO);
                glBindVertexArray(VAO);

                GLuint VBO;
                glGenBuffers(1, &VBO);
                glBindBuffer(GL_ARRAY_BUFFER, VBO);

                GLuint EBO;
                glGenBuffers(1, &EBO);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

                // coordinates are specified in default "front-facing" order, counter clockwise
                GLfloat buffer_data[] = {
                    // xy coords    // colors
                    +0.0f, +0.5f,   1.0f, 0.0f, 0.0f,
                    -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,
                    +0.5f, -0.5f,   0.0f, 0.0f, 1.0f,
                };
                buffer_data[1] = -0.5f + glm::sqrt((1 + -0.5f - 0.0f) * (1 - -0.5 + 0.0f)); // distance formula re-arrangement to make an equilateral triangle

                GLuint index_data[] = {
                    0, 1, 2
                };

                glBufferData(GL_ARRAY_BUFFER, sizeof(buffer_data), buffer_data, GL_STATIC_DRAW);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);

                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(buffer_data) / 3, (void*) (sizeof(GLfloat) * (&buffer_data[0] - buffer_data)));
                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(buffer_data) / 3, (void*) (sizeof(GLfloat) * (&buffer_data[2] - buffer_data)));
                glEnableVertexAttribArray(0);
                glEnableVertexAttribArray(1);

                glBindVertexArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

                GLint location = glGetUniformLocation(shader_program, "matrix");

                glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
                while (glfwWindowShouldClose(window) != GLFW_TRUE) {
                    glClear(GL_COLOR_BUFFER_BIT);

                    glUseProgram(shader_program);

                    glm::mat4 matrix(1.0f);
                    matrix = glm::rotate(matrix, (float) glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

                    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));

                    glBindVertexArray(VAO);
                    glDrawElements(GL_TRIANGLES, sizeof(index_data) / sizeof(GLuint), GL_UNSIGNED_INT, nullptr);

                    glfwSwapBuffers(window);
                    glfwPollEvents();
                }

                glDeleteShader(vertex_shader);
                glDeleteShader(fragment_shader);
                glDeleteProgram(shader_program);

                glDeleteVertexArrays(1, &VAO);
                glDeleteBuffers(1, &VBO);
                glDeleteBuffers(1, &EBO);
            }

            glfwDestroyWindow(window);
        }

        glfwTerminate();
    }

    return -1;
}
