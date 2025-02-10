// write the code to generate a sample window

#include <gl/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

#include <iostream>

// set some hints, or specifics - GLFW should follow when creating a window

// set window size callback

void windowSizeCallback(GLFWwindow *window, int width, int height)
{
    std::cout << "Window resized to width: " << width << " and height: " << height << std::endl;
    glViewport(0, 0, width, height);
}

// glViewport tells which rectangular area of the window to render to

int main()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // openGL version 3.3

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // disable deprecated functionality
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // forward compatibility

    // now hints are set, we can create a window with the function GLFWwindow* glfwCreateWindow(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)

    GLFWwindow *window = glfwCreateWindow(500, 500, "OpenGL Window", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        return -1;
    }

    // make the window's context current
    glfwMakeContextCurrent(window);

    // initialize GLEW, which must be done for each OpenGL context
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cout << "GLEW failed to initialize" << std::endl;
        return -1;
    }

    // add callback
    glfwSetWindowSizeCallback(window, windowSizeCallback);

    // now make a basic triangle
    std::vector<glm::vec3> vertices; // vector of 3D vectors
    // glm:vec3 is the template type parameter

    // add render loop

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents(); // call the callbacks for any events that have been triggered
        glfwSwapBuffers(window);
    }

    //  terminate glfw
    glfwTerminate();
    return 0;
}
