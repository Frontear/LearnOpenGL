#ifndef FRONTEAR_VERTEX_ARRAY
#define FRONTEAR_VERTEX_ARRAY

#include <tuple>
#include <vector>
#include <GL/glew.h>

namespace frontear {
    class vert_array {
    public:
        GLuint ID;
        std::vector<std::tuple<GLuint, GLenum>> buffers;

    public:
        explicit vert_array() {
            glGenVertexArrays(1, &ID);
        }

        ~vert_array() {
            for (const auto &[id, target] : buffers) {
                glBindBuffer(target, 0);
                glDeleteBuffers(1, &id);
            }

            buffers.clear();

            glBindVertexArray(0);
            glDeleteVertexArrays(1, &ID);
        }

        template <class T>
        void buffer(GLenum target, T &data, GLenum usage) {
            glBindVertexArray(ID);

            GLuint id;
            glGenBuffers(1, &id);
            glBindBuffer(target, id);
            glBufferData(target, sizeof(data), data, usage);

            glBindVertexArray(0);

            buffers.push_back(std::tuple<GLuint, GLenum>(id, target));
        }

        void attrib(GLuint index, GLint size, GLsizei stride, const void* pointer) {
            glBindVertexArray(ID);

            glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
            glEnableVertexAttribArray(index);

            glBindVertexArray(0);
        }

        void use() {
            glBindVertexArray(ID);
        }
    };
};

#endif
