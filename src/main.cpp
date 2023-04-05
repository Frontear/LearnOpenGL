#define GLFW_INCLUDE_NONE

#include <string>
#include <cstddef>
#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main() {
    // INITIALIZE

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, false);

    GLFWwindow* window = glfwCreateWindow(854, 480, "LearnOpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    glewInit();

    GLsizei width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // SHADERS
    std::ifstream vert_file("assets/default.vert");
    std::ifstream frag_file("assets/default.frag");

    std::ostringstream vert_strm;
    std::ostringstream frag_strm;

    vert_file >> vert_strm.rdbuf();
    frag_file >> frag_strm.rdbuf();

    std::string vert_str = vert_strm.str();
    std::string frag_str = frag_strm.str();

    const GLchar* vert_content = vert_str.c_str();
    const GLchar* frag_content = frag_str.c_str();

    GLuint vert_id;
    vert_id = glCreateShader(GL_VERTEX_SHADER);

    GLuint frag_id;
    frag_id = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vert_id, 1, &vert_content, nullptr);
    glShaderSource(frag_id, 1, &frag_content, nullptr);

    glCompileShader(vert_id);
    glCompileShader(frag_id);

    GLuint program_id;
    program_id = glCreateProgram();

    glAttachShader(program_id, vert_id);
    glAttachShader(program_id, frag_id);

    glLinkProgram(program_id);

    // BUFFERS (ALL COORDS ARE PROCESSED COUNTER-CLOCKWISE)

    struct vertex {
        GLfloat xy[2];
        GLfloat rgb[3];
    };

    vertex vbo_data[] = {
        // location 0 vec2 xy   // location 1 vec3 rgb
        { { -0.5, +0.5 },       { 1.0, 0.5, 0.5 } },
        { { -0.5, -0.5 },       { 1.0, 1.0, 0.5 } },
        { { +0.5, -0.5 },       { 0.5, 0.5, 1.0 } },
        { { +0.5, +0.5 },       { 0.5, 1.0, 0.5 } },
    };

    GLuint ebo_data[] = {
        0, 1, 2,
        0, 2, 3,
    };

    GLuint vbo_id;
    glCreateBuffers(1, &vbo_id);

    GLuint ebo_id;
    glCreateBuffers(1, &ebo_id);

    GLuint vao_id;
    glGenVertexArrays(1, &vao_id);

    glNamedBufferData(vbo_id, sizeof(vbo_data), vbo_data, GL_STATIC_DRAW);
    glNamedBufferData(ebo_id, sizeof(ebo_data), ebo_data, GL_STATIC_DRAW);

    glBindVertexArray(vao_id); // MUST BIND: https://gamedev.stackexchange.com/a/151565
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);

    glVertexAttribPointer(0, sizeof(vertex::xy) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, xy));
    glVertexAttribPointer(1, sizeof(vertex::rgb) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, rgb));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // RENDER LOOP

    glUseProgram(program_id);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
            break;
        }

        glfwPollEvents();
    }

    // CLEANUP

    glDeleteVertexArrays(1, &vao_id);
    glDeleteBuffers(1, &ebo_id);
    glDeleteBuffers(1, &vbo_id);

    glDeleteProgram(program_id);
    glDeleteShader(frag_id);
    glDeleteShader(vert_id);

    glfwDestroyWindow(window);

    glfwTerminate();
}
