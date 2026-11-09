#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>

void error_callback(int error, const char* description)
{

    std::fprintf(stderr, "Error %d: %s\n", error, description);
}

int main()
{
    glfwSetErrorCallback(error_callback);

    glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);

    if (!glfwInit()) {
        std::fprintf(stderr, "Error: GLFW initialization failed!\n");
        return EXIT_FAILURE;
    }

    GLFWwindow* window;

    window = glfwCreateWindow(640, 480, "Space Invaders", nullptr, nullptr);

    if (window == nullptr) {

        std::fprintf(stderr, "Error: Window creation failed!\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    GLenum glew_status = glewInit();

    if (glew_status != GLEW_OK) {
        std::fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
        glfwTerminate();
        return EXIT_FAILURE;
    }

    std::fprintf(stdout, "OpenGL-Version: %s\n", glGetString(GL_VERSION));

    // Game Loop
    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    // Hier Aufräumen
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
