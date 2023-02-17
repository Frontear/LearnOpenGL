#include <iostream>
#include <GL/glew.h> // must come before so it can setup the GL headers for GLFW
#include <GLFW/glfw3.h>

void resizeWindowCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main() {
    glfwInit();
    // versioning is M.m, where M is MAJOR and m is minor. So we set to 3.3. I think?
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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
    glfwSetFramebufferSizeCallback(window, resizeWindowCallback); // must be done since viewport depends on height/width, so if it is resized, the viewport must be set again accordingly

    // simple render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window); // the logic here depends on poll events. so I wonder why he puts it here instead of after it, is it so the app can get one last render tick before it actually handles inputs? or i guess, inputs can possibly affect rendering? like pressing W can cause you to render differently down the line, so it just makes sense to check pre-render. idk i wonder if its noticeable. yeah the logic here is that the next render frame will be affected by the logic of the key-press. tbh though regardless of where we check it would anyways, like if we check after the events are only re-polled after the loop so it would still remain at the same state. idk doesn't matter i guess

        glClearColor(0.2, 0.3, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT); // clears the data in the color buffers, sets it to default values, which we are overwriting above by saying the new clear values are.. that color

        glfwSwapBuffers(window); // a buffer is a large 2d collection of sorts that contains color data for each pixel on the window. ive no clue why the swap though
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}

void resizeWindowCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
