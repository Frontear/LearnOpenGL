#ifndef FRONTEAR_VERTEX_ARRAY
#define FRONTEAR_VERTEX_ARRAY

#include <tuple>
#include <vector>
#include <GL/glew.h>

namespace frontear {
    class vert_array {
    private:
        GLuint ID;
        std::vector<std::tuple<GLuint, GLenum>> buffers;

    public:
        explicit vert_array() {
            glGenVertexArrays(1, &ID);
        }

        ~vert_array() {
            for (const auto &[id, target] : buffers) {
                glDeleteBuffers(1, &id);
            }

            buffers.clear();
            glDeleteVertexArrays(1, &ID);
        }

        template <class T>
        void buffer(GLenum target, T &data, GLenum usage) {
            GLuint id;
            glGenBuffers(1, &id);
            glBindBuffer(target, id);
            glBufferData(target, sizeof(data), data, usage);

            buffers.push_back(std::tuple<GLuint, GLenum>(id, target));
        }

        void attrib(GLuint index, GLint size, GLsizei stride, const void* pointer) {
            glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
            glEnableVertexAttribArray(index);
        }

        void use() {
            glBindVertexArray(ID);
        }
    };
};

#endif
