#ifndef FRONTEAR_VERTEX_ARRAY
#define FRONTEAR_VERTEX_ARRAY

#include <tuple>
#include <vector>
#include <GL/glew.h>

namespace frontear {
    class vert_array {
    private:
        GLuint ID;
        GLsizei stride;
        std::vector<std::tuple<GLuint, GLenum>> buffers;

    public:
        explicit vert_array() {
            glGenVertexArrays(1, &ID);
        }

        ~vert_array() {
            for (const auto &[id, _] : buffers) {
                glDeleteBuffers(1, &id);
            }

            buffers.clear();
            glDeleteVertexArrays(1, &ID);
        }

        template <class T>
        void buffer(GLenum target, T &data, GLenum usage) {
            glBindVertexArray(ID);

            GLuint id;
            glGenBuffers(1, &id);
            buffers.push_back(std::tuple<GLuint, GLenum>(id, target));

            glBindBuffer(target, id);
            glBufferData(target, sizeof(data), data, usage);

        }

        template <class T>
        void vert_buffer(GLint count, T &data, GLenum usage) {
            glBindVertexArray(ID);

            GLuint id;
            glGenBuffers(1, &id);
            buffers.push_back(std::tuple<GLuint, GLenum>(id, GL_ARRAY_BUFFER));

            glBindBuffer(GL_ARRAY_BUFFER, id);
            glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, usage);

            this->stride = sizeof(data) / count;
        }

        template <size_t N>
        void elem_buffer(GLuint (&data)[N], GLenum usage) {
            glBindVertexArray(ID);

            GLuint id;
            glGenBuffers(1, &id);
            buffers.push_back(std::tuple<GLuint, GLenum>(id, GL_ELEMENT_ARRAY_BUFFER));

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(data), data, usage);
        }

        void attrib(GLuint index, GLint size, const void* pointer) {
            glBindVertexArray(ID);

            glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
            glEnableVertexAttribArray(index);
        }

        void use() {
            glBindVertexArray(ID);
        }
    };
};

#endif
