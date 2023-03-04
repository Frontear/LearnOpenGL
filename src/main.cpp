#ifdef _WIN32
#define GLFW_DLL
#endif

#include <iostream>
#include <GL/glew.h> // must come before so it can setup the GL headers for GLFW
#include <GLFW/glfw3.h>

void processInput(GLFWwindow* window);

int main() {
    glfwInit();
    // versioning is M.m, where M is MAJOR and m is minor. So we set to 3.3. I think?
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // prevents usage of all the immediate pipeline shenanigans, basically forces us to use the modern stuff. this is a good option to set
    #ifdef __APPLE__ && __MACH__ // macOS check: https://stackoverflow.com/questions/142508/how-do-i-check-os-with-a-preprocessor-directive
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // TODO: does the window size here set some internal max? I notice when I resize past 1280x720 (not exactly past it, but around there) the window borders artifact. could be a graphics glitch on my laptop specifically, making it smaller and returning it to almost original size doesnt artifact though..?
    // when i changed it to 800x600, leaving it as that now, it doesnt artifact at all? so it is a PC issue :p
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) { // would !window work here? also checks if window was created successfully
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) { // here is where we deviate from the tutorial. this should realistically be the only thing we aren't doing the same. feel free to use GLAD one day instead idc, i dont know the difference as it is
        std::cout << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, 800, 600); // first 2 params set the coordinates for the bottom left corner, last 2 params i imagine are for the top right
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }); // must be done since viewport depends on height/width, so if it is resized, the viewport must be set again accordingly

    // version 330 core corresponds to our GL version. we set to 3.3 earlier. the core part mentions we are using core profile
    // in means input vertex attributes. there's probably a few attributes here you can set, but right now we only wanted position.
    // vec4 is xyzw, but it doesnt mean 4D, nor does w mean anything weird like that. w actually means something called perspective division
    // setting gl_Position inside of the main function is what we take as the position data. That's the output/handling of the vertex data, otherwise the position would not be set.
    // there's a special reason why w = 1.0, not explained yet, but to keep in mind. its not alpha fyi, its perspective division, whatever that is
    // in this vertex shader the gl_Position is expected to be normalized, meaning ranges are shrunk to [-1.0, 1.0]. since our vertex is already normalized we dont process here, but normally you'd either have to check, or just process.
    const char* vshader = R"(
        #version 330 core
        layout (location = 0) in vec2 pos;
        uniform vec2 mouse;

        void main() {
            gl_Position = vec4(pos.x + mouse.x, pos.y - mouse.y, 0.0, 1.0); // mouse movements here aren't perfectly 1:1, because this shader is run per vertex, so logically all vertex's are being affected by mouse.x, not just the top left like I expected at first
        }
    )";
    unsigned int vso;
    vso = glCreateShader(GL_VERTEX_SHADER); // does the same as before with the vbo, generates an id for the shader, we tell GL its a vertex shader specifically, and can use it later to process. I just wonder why they didnt do glCreateShader(GL_VERTEX_SHADER, &vso); seems like a weird choice to make it diff here since the object vso and vbo both are just IDs. shrug
    glShaderSource(vso, 1, &vshader, nullptr); // the 1 represents how many string we are passing into the function. i wonder what cases you'd rather pass a string as multiple anyways for that to be an option. anyways this function sets the shader to our shader id from earlier.
    glCompileShader(vso);

    int success;
    char log[1024];
    glGetShaderiv(vso, GL_COMPILE_STATUS, &success); // checks if the shader compiled at all, kind of important if i end up wanting to experiment with shaders

    if (!success) {
        glGetShaderInfoLog(vso, 1024, nullptr, log); // why nullptr here, idk
        std::cout << "Error compiling Vertex Shader:\n" << log << std::endl;
        glfwTerminate();
        return -1;
    }

    // fragment shaders only have 1 output variable, the color, which is a vec4 of rgba. i wonder why we dont do out vec4 gl_Position in the vertex shader. I think its because gl_Position is already defined, whereas here we need to tell opengl we are giving an vec4 out, which opengl will always treat as a color.
    const char* fshader = R"(
        #version 330 core
        layout(origin_upper_left) in vec4 gl_FragCoord;
        out vec4 color;

        void main() {
            float pink_r = 255 / 255.0f;
            float pink_g = 192 / 255.0f;
            float pink_b = 203 / 255.0f;

            float blue_r = 65 / 255.0f;
            float blue_g = 105 / 255.0f;
            float blue_b = 255 / 255.0f;

            // lerping here to get gradient fill across shape
            // 800 from window size
            float r = pink_r - (pink_r - blue_r) * gl_FragCoord.x / 800.0f;
            float g = pink_g - (pink_g - blue_g) * gl_FragCoord.x / 800.0f;
            float b = pink_b - (pink_b - blue_b) * gl_FragCoord.x / 800.0f;

            color = vec4(0.25 + r, g, b, 1.0);
        }
    )";
    unsigned int fso;
    fso = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fso, 1, &fshader, nullptr);
    glCompileShader(fso);

    //int success;
    //char log[1024];
    glGetShaderiv(fso, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fso, 1024, nullptr, log);
        std::cout << "Error compiling Fragment Shader:\n" << log << std::endl;
        glfwTerminate();
        return -1;
    }

    unsigned int sp;
    sp = glCreateProgram(); // a shader program is a collection of your shaders, you set the program prior to rendering to tell opengl to use your shaders defined in the program during the rendering process. this way you can have multiple shaders all bound to specific programs, maybe a shader program for night time, day time, if there's an event happening of some kind idk. cool ideas
    glAttachShader(sp, vso);
    glAttachShader(sp, fso);
    glLinkProgram(sp); // links the objects to the shader i think. this supposedly also means we need to dispose of our shader objects from earlier, since they are now linked to the program and no longer need a reference. not sure internally whether that means it copies the shaders to the program idk. anyways this is a finalizing step for the program, it needs to be done to slap all those shaders together into the program.

    //int success;
    //char log[1024];
    glGetProgramiv(sp, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(sp, 1024, nullptr, log);
        std::cout << "Error linking Shader Program:\n" << log << std::endl;
        glfwTerminate();
        return -1;
    }

    // so im not quite sure how the link process works, but apparently once the link happens, these objects are no longer necessary while the shader program can still hold a reference to them in some way. idk how it works, but regardless, once linked, the shader objects can be wiped.
    glDeleteShader(vso);
    glDeleteShader(fso);

    // a VAO saves the state of a vertex with its attributes + binds it to the associated VBO. in our case, it will save the data we set in attrib pointer and enable attrib array, it will know that we are using our current VBO for those attribs, so that whenever we bind our vertex array to this VAO, we can just re-use these functions without needing to set them again.
    // a good way to think of a VAO is that it defines how GL should translate the data from a VBO into a vertex shader.
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao); // now any changes we make to the vertex attrib pointer or vbo will be reflected in our VAO

    // vertices for a square, we will define how it should be handled via the EBO
    float vertices[] = {
        -1.0,   1.0,
         0.0,   1.0,
        -1.0,   0.0,
         0.0,   0.0,
    };

    unsigned int vbo; // a vbo is an object that just contains vertex data, so the coords above, can then be saved to the vbo and saved into the GPU
    glGenBuffers(1, &vbo); // this is just generating a buffer. GL doesn't know this is a VBO yet, only that this is some type of buffer. also this isn't actually allocating buffer memory yet, but rather just giving the buffer an ID, i think. That's why we can put a sizeof(vertices) later since we need to know the size to allocate for the buffer later, and not now. also would explain why the type of a vbo is just an unsigned int :p
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // so apparently GL_ARRAY_BUFFER means vertex buffer, i guess because vertices are stored as float arrays, like i did earlier? so now GL knows this is a vertex buffer object specifically. that being said, it means that our currently set VBO is the vbo i generated earlier. we can still manage other buffers, just as long as they are NOT GL_ARRAY_BUFFER, or vertex buffer objects, since we would then need to rebind it to that vbo instead.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // fairly trivial, we are setting that previously bound VBO to now actually have our vertex data. the last bit, GL_STATIC_DRAW, supposedly means that the data gets set once, and is used many times. This is correct for our use case. there's also GL_DYNAMIC_DRAW, which both changes a lot and draws a lot, and GL_STREAM_DRAW, which is set once and used a few times, not too much. This specific value helps the GPU know how the data should be managed. If its STATIC or DYNAMIC i imagine GPUs give it more priority since its an important rendering component. of course, this is implementation specific.

    // visualized this on desmos, but the idea is that we draw triangles using our coords. supposedly, opengl usually works best with triangles. wonder how we render complex shapes like scribbles as triangles. oh well :p
    unsigned int indices[] = {
        0,  1,   2,
        1,  2,   3
    };

    // an EBO, or element buffer object, is an object that tells GL what indices to use from our vertex coordinates when drawing. We need to specify as many indices as the theoretical vertex coordinates of our object. In the case of my simple triangle its still 3, but lets say we wanted to make a shape like a square, and we did it with 2 triangles. We can define the four corners of the triangle, and then create an EBO to define 2 triangle vertices, so 6 indices, using the 4 vertices from prior.
    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // ELEMENT_ARRAY_BUFFER tells GL this is an EBO.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // lots of cool stuff here, first parameter is trying to specify which vertex attribute we wanna modify. earlier we set our location = 0 in the vertex shader, which corresponds to the 0th vertex attribute. hence, here, we use 0
    // the 3 tells GL that we have 3 coordinates to work with in our vertex. the tutorial notes that since we use a vec3 we send 3. I'm not sure if this implies the vec3 pos in the vertex shader shader, or the vertices array we had instead. I can confirm now it is NOT the 3 from either the vertex or the indices, but rather from the vec3 position we use in our vertex shader.
    // GL_FLOAT is the type of our vertices, all floats. Any vec type is a float, so vec3, vec4 etc
    // GL_FALSE tells GL we dont want to normalize our data. this is interesting because i was led to believe that vertex shaders should calculate normalization, whereas here it seems GL does too? unless this is a separate normalization that doesn't affect the shaders, but that doesn't make sense since this tells GL how to process the vertex data and communicate it to our shaders.. idk. Tried to mess with this and i have NO idea what it does lol.
    // 4 * sizeof(float) looks spooky, but basically it tells GL the size of one coordinate in the array block. our coordinates are 4, xyzw, so we tell GL that each coordinate is 4 floats long, so it knows where the coordinates start and end. This is in respect to the way our VBO/vertices are written.
    // 0 here tells GL to start looking for coordinates from the 0th position, or the start of the array.
    // this function takes its data from the managed VBO, the one bound to GL_ARRAY_BUFFER. For our vertex shaders, the 0th attribute is now bound to that VBO. we could rebind the VBO and set it to 1 instead or something to have multiple vertex attributes, and obviously would need handling in the vertex shaders accordingly. it is set PER VBO that is BOUND, so if youre trying to set another VBO, BIND IT FIRST, AND MAKE SURE TO UNBIND IF YOURE SETTING ANOTHER ONE
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0); // enables the given vertex attribute, since our VBO is bound to attribute 0, and we set it earlier, this can be set here, attribute here is the layout 0 thing
    glBindVertexArray(0); // removes the currently active VAO, should be fine since we rebind it in our render loop
    // can unbind these two as well because the bindings are associated now via the VAO.
    glBindBuffer(GL_VERTEX_ARRAY, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // simple render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window); // the logic here depends on poll events. so I wonder why he puts it here instead of after it, is it so the app can get one last render tick before it actually handles inputs? or i guess, inputs can possibly affect rendering? like pressing W can cause you to render differently down the line, so it just makes sense to check pre-render. idk i wonder if its noticeable. yeah the logic here is that the next render frame will be affected by the logic of the key-press. tbh though regardless of where we check it would anyways, like if we check after the events are only re-polled after the loop so it would still remain at the same state. idk doesn't matter i guess

        glClearColor(0.2, 0.3, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT); // clears the data in the color buffers, sets it to default values, which we are overwriting above by saying the new clear values are.. that color

        glUseProgram(sp); // now we tell opengl we want to use that specific program and the associated shaders. this can be set inside the loop as well, we dont need to do that since we only have one shader program

        unsigned int mouse;
        mouse = glGetUniformLocation(sp, "mouse");

        // processing of coords
        double pos[2];
        int size[2];
        glfwGetCursorPos(window, &pos[0], &pos[1]);
        glfwGetWindowSize(window, &size[0], &size[1]);

        float mousePos[] = {
            // normalizing
            pos[0] / size[0],
            pos[1] / size[1]
        };
        glUniform2fv(mouse, 1, mousePos);

        glBindVertexArray(vao); // bind to our VAO and restore the attrib pointer data and vbo associated with it
        //glDrawArrays(GL_TRIANGLES, 0, 3); // telling GL here we want to render a TRIANGLE, 0 is the starting index of our vertices, and 3 is saying how many vertices we want to draw. obviously we want to draw all 3 of our vertices
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // we tell GL we are rendering from an EBO, 6 is our coordinates, GL_UNSIGNED_INT is the data type, and 0 is the start index of our ebo array.

        glfwSwapBuffers(window); // a buffer is a large 2d collection of sorts that contains color data for each pixel on the window. ive no clue why the swap though
        glfwPollEvents(); // input events, mouse events, click events, drag events, resize events ETC ETC
    }

    glfwTerminate();

    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
