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

    // initializes core glfw contexts
    glfwInit();

    // opengl 4.6 with core profile forced
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 4x anti aliasing and preventing resizing to stop my WM from maxing it out (if i find a different way to prevent it i will remove this flag)
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, false);

    // creates a glfw window and creates an associated opengl context for it. binds the context to the main, allowing gl helpers like glew, glad, and others to load and work
    GLFWwindow* window = glfwCreateWindow(854, 480, "LearnOpenGL", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // enable experimental glew features (mostly for core profile) and start glew
    glewExperimental = true;
    glewInit();

    // pull the framebuffer size for glViewport, giving gl the viewing size for rendering, which is the entire window
    GLsizei width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // SHADERS

    // open two files to the associated shader files
    std::ifstream vert_file("assets/default.vert");
    std::ifstream frag_file("assets/default.frag");

    // create an ostringstream for both shaders to make it easier to pull data into a char*
    std::ostringstream vert_strm;
    std::ostringstream frag_strm;

    // push down file content into the stream, all the way to EOL
    vert_file >> vert_strm.rdbuf();
    frag_file >> frag_strm.rdbuf();

    // convert to C++ string and hold reference on stack, mandatory for the next step
    std::string vert_str = vert_strm.str();
    std::string frag_str = frag_strm.str();

    // pull char* from the C++ string. the string needs a hard ref on stack otherwise it wont have a valid reference to the char*
    const GLchar* vert_content = vert_str.c_str();
    const GLchar* frag_content = frag_str.c_str();

    // generate the shaders on gl, giving an associated id for us to store data.
    GLuint vert_id;
    vert_id = glCreateShader(GL_VERTEX_SHADER);

    GLuint frag_id;
    frag_id = glCreateShader(GL_FRAGMENT_SHADER);

    // upload shader content from file to the associated shader
    glShaderSource(vert_id, 1, &vert_content, nullptr);
    glShaderSource(frag_id, 1, &frag_content, nullptr);

    // run a compile on the shader, necessary step before linking to ensure the shader is valid
    glCompileShader(vert_id);
    glCompileShader(frag_id);

    // create a shader program to attach all our shaders onto
    GLuint program_id;
    program_id = glCreateProgram();

    // attach both the shaders to the shader program
    glAttachShader(program_id, vert_id);
    glAttachShader(program_id, frag_id);

    // link the program, finalizing all attachments and linking the shaders to one another in the shader pipeline (vert -> frag)
    glLinkProgram(program_id);

    // BUFFERS (ALL COORDS ARE PROCESSED COUNTER-CLOCKWISE)

    // simple structure to store vertex information for the vertex shader
    struct vertex {
        GLfloat xy[2];
        GLfloat rgb[3];
    };

    // store 4 vertices, 2d space coordinates and rgb values for each coordinate
    vertex vbo_data[] = {
        // location 0 vec2 xy   // location 1 vec3 rgb
        { { -0.5, +0.5 },       { 1.0, 0.5, 0.5 } },
        { { -0.5, -0.5 },       { 1.0, 1.0, 0.5 } },
        { { +0.5, -0.5 },       { 0.5, 0.5, 1.0 } },
        { { +0.5, +0.5 },       { 0.5, 1.0, 0.5 } },
    };

    // store 6 index references for rendering of the VBO, 3 coordinate pairs because GL renders off triangles, indexes refer to the vertices
    GLuint ebo_data[] = {
        0, 1, 2,
        0, 2, 3,
    };

    // generate a VBO, an object that store vertex info, an EBO, an object that stores information on how to access the VBO for rendering purposes, and a VAO, an object that stores not only all binded buffers, but attrib data for how each buffer is accessed in the vertex shader
    GLuint vbo_id;
    // usage of create here is intentional and very different from a simple glGen, glCreate actually instantiates the object and allows GL to manipulate it immediately, whereas glGen only actually makes the object once it gets binded via glBindBuffer. We are NOT binding early, just for visual sake, so we use create
    glCreateBuffers(1, &vbo_id);

    GLuint ebo_id;
    glCreateBuffers(1, &ebo_id);

    GLuint vao_id;
    glGenVertexArrays(1, &vao_id);

    // store vertex data in the VBO, and index data in the EBO. both are labelled as STATIC_DRAW, because they will both be used for rendering, data will not change (STATIC), and will be used many times (DRAW)
    glNamedBufferData(vbo_id, sizeof(vbo_data), vbo_data, GL_STATIC_DRAW);
    glNamedBufferData(ebo_id, sizeof(ebo_data), ebo_data, GL_STATIC_DRAW);

    // bind a VAO, this will subsequently store information about all binded buffers, the VBO and EBO. This will also store information about the vertex attrib data, when can then all be easily set by binding the VAO later at any time
    glBindVertexArray(vao_id); // MUST BIND: https://gamedev.stackexchange.com/a/151565
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);

    // informs the vertex shader how to process the VBO. Vertex shaders must have location based in variables, defined as (location = 0) or (location = 1). GL can't necessarily reliably know how the data in the buffer correlates (because a buffer can have absolutely any sort of data, GL can't assume you'll write it to spec for the vertex shader), so we use this to set information. We tell GL the location of the input, the amount of data in it (in our case 2 floats), the fact that it is a float via GL_FLOAT, GL_FALSE for normalizing because we already normalized our data between [-1, 1], give the sizeof the vertex as the stride, aka how long each singular vertex is. For other, more primitive VBO data types, like a simple GLfloat[], you would need to specify how long each individual vertex is meant to be more manually, and finally the offset of the data with respect to the beginning of the VBO memory block, which for this we can use offsetof to easily give us that information
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, xy));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*) offsetof(vertex, rgb));

    // enable them. I don't know why this isn't automatically done, im sure there is a reason
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // RENDER LOOP

    // enable our shader program to use our shaders in the rendering pipeline
    glUseProgram(program_id);

    // enable the VAO to bring back all our buffers and attrib data, then draw the elements from our EBO. We defined it as a pair of triangles so we specify triangles, then the amount of indices in the EBO, what type each index is, and then finally a nullptr because GL pulls our index data from the EBO. If we did not have an EBO we could give it an array here. Does make me wonder why we bother making an EBO, maybe so it can be binded to a VAO that has a VBO with it
    glBindVertexArray(vao_id);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // flip the buffers, bringing the front empty one back, and the back one which GL has been rendering on to the front, effectively letting us see what we wanted to render
    glfwSwapBuffers(window);

    // constant loop while the glfw window remains open
    while (!glfwWindowShouldClose(window)) {
        // checks if the escape key is pressed, and if it is fast close the window
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
            break;
        }

        // poll key events, mouse events, and maybe other things that idk
        glfwPollEvents();
    }

    // CLEANUP

    // wipe all our objects from memory. this is honestly not necessary because after program termination the OS reclaims the memory on its own, but its good practice to know this if you are gonna make a limited lifetime gl object, or anything else.
    glDeleteVertexArrays(1, &vao_id);
    glDeleteBuffers(1, &ebo_id);
    glDeleteBuffers(1, &vbo_id);

    glDeleteProgram(program_id);
    glDeleteShader(frag_id);
    glDeleteShader(vert_id);

    // destroy the glfw window and the associated context with it.
    glfwDestroyWindow(window);

    // terminate the glfw library, again not neccessary but its beneficial to undo any settings glfw may have changed on your system prior when it init. see the glfw docs for more details on what this means.
    glfwTerminate();
}
