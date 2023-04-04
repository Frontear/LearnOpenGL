#ifndef FRONTEAR_BUFFER
#define FRONTEAR_BUFFER

#include <GL/glew.h>

namespace frontear {
    class buffer {
    public:
        GLuint ID;
        GLenum target;

    public:
        explicit buffer(GLenum t) : target(t) {
            glGenBuffers(1, &ID);
        }

        ~buffer() {
            glBindBuffer(target, 0);
            glDeleteBuffers(1, &ID);
        }

        template <class T>
        void upload(T &data, GLenum usage) {
            glBindBuffer(target, ID);
            glBufferData(target, sizeof(data), data, usage);
        }
    };
};

#endif
