// write the code to generate a sample window

#include <GL/glew.h>
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
    glfwInit(); // initialize the library
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

    // now make a basic triangle
    std::vector<glm::vec3> vertices; // vector of 3D vectors
    // glm:vec3 is the template type parameter for std::vector, telling it what it will be holding

    vertices.push_back(glm::vec3(-0.75f, -0.65f, 0.f));
    vertices.push_back(glm::vec3(0.75f, -0.65f, 0.f));
    vertices.push_back(glm::vec3(0.f, 0.75f, 0.f));

    // introducing three points to connect in order to make a triangle

    // the drawing area in OpenGL is from -1 to 1 in all directions
    // the z-axis is depth, which we are not interested in right now

    // assign a colour for each object, again using glm::vec3
    std::vector<glm::vec3> colours;

    colours.push_back(glm::vec3(1.f, 0.f, 0.f)); // red
    colours.push_back(glm::vec3(0.f, 1.f, 0.f)); // green
    colours.push_back(glm::vec3(0.f, 0.f, 1.f)); // blue

    // these two vectors present a coloured triangle on the CPU

    // now we need to send this data to the GPU
    // VBOs store vertex data
    // VAOs store the state of the vertex data and track VBOs

    // for our triangle we have two vertex attributes - we need one VAO and two VBOs

    GLuint vao;
    glGenVertexArrays(1, &vao); // generate a VAO
    glBindVertexArray(vao);     // bind the VAO (make it active)

    // create VBOs (one for positions and one for colours)
    GLuint vertexBuff;
    GLuint colourBuff;

    glGenBuffers(1, &vertexBuff); // generate a VBO for the vertices
    glGenBuffers(1, &colourBuff); // generate a VBO for the colours

    // activate VBOs and set attribute pointers
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuff);                     // bind the VBO for the vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0); // usage of the void pointer means that the data is tightly packed

    glEnableVertexAttribArray(0); // enable the vertex attributes

    glBindBuffer(GL_ARRAY_BUFFER, colourBuff); // bind the VBO for the colours
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    // enable the vertex attributes
    glEnableVertexAttribArray(1);

    // now the GPU knows where to look for and how to interpret our triangle data

    // but, we still need to send the data to the GPU
    // use glBufferData(target, size, data, usage) to send data to the GPU
    // data points to size bytes that will be transferred to the target buffer object

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuff);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colourBuff);
    glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(glm::vec3), colours.data(), GL_STATIC_DRAW);

    // now we have sent the data to the GPU, ready to be rendered

    // except we need to tell the GPU how to render the data = shaders
    // two types: vertex (on vertices) and fragment shaders (on pixels)

    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout(location = 0) in vec3 position;\n"
                                     "layout(location = 1) in vec3 colour;\n"

                                     "out vec3 fragColour;\n"
                                     // all this does is get the colour from the vertex shader and pass it to the fragment shader

                                     "void main() {\n"
                                     "   gl_Position = vec4(position, 1.0);\n"
                                     "C = colour;\n"
                                     "}\n";

    const char *fragmentShaderSource = "#version 330 core\n"
                                       "in vec3 C;\n"
                                       "out vec4 colour;\n"

                                       "void main() {\n"
                                       "   colour = vec4(C, 1.0f);\n"
                                       "}\n";
    // these shaders just render the positions and colours of the vertices directly

    // create the shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);     // create a handler
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // attach the source code to the handler
    glCompileShader(vertexShader);                              // compile the shader

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // create a shader program t0 make sure compilation is successful
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLint logLength;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logLength);   // get the length of the log
        std::vector<char> log(logLength);                              // create a vector to store the log
        glGetShaderInfoLog(vertexShader, logLength, NULL, log.data()); // get the log

        std::cerr << "Error compiling vertex shader: " << log.data() << std::endl;
    }

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLint logLength;
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetShaderInfoLog(fragmentShader, logLength, NULL, log.data());

        std::cerr << "Error compiling fragment shader: " << log.data() << std::endl;
    }

    // let's assume the vertex and fragment shader both compile, finally we link them together to create a shader program
    GLuint programID = glCreateProgram();      // create a shader program
    glAttachShader(programID, vertexShader);   // attach the vertex shader
    glAttachShader(programID, fragmentShader); // attach the fragment shader
    glLinkProgram(programID);                  // link the shaders

    // check if the program linked successfully
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success)
    {
        GLint logLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<char> log(logLength);
        glGetProgramInfoLog(programID, logLength, NULL, log.data());

        std::cerr << "Error linking program: " << log.data() << std::endl;
    }

    // add render loop

    // advanced:
    // 1. Tell the GPU to use the shader program
    // 2. Bind the VAO
    // 3. Enable GL_SRGB_FRAMEBUFFER to ensure that the colours are displayed correctly
    // 4. Clear the previous frame
    // 5. Draw the triangle

    // add callback
    glfwSetWindowSizeCallback(window, windowSizeCallback);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();              // call the callbacks for any events that have been triggered
        glUseProgram(programID);       // use our shader
        glBindVertexArray(vao);        // bind the VAO
        glEnable(GL_FRAMEBUFFER_SRGB); // enable sRGB framebuffer

        // clear previous contents
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the colour and depth buffers

        glDrawArrays(GL_TRIANGLES, 0, 3); // draw the triangle

        // other parts of the code MAY require sRGB to be disabled, for now no though
        // glDisable(GL_FRAMEBUFFER_SRGB);

        glfwSwapBuffers(window);
    }

    // practicing good habits, clean up after yourself
    glDeleteBuffers(1, &vertexBuff);
    glDeleteBuffers(1, &colourBuff);
    glDeleteVertexArrays(1, &vao);

    // also cleanup the shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(programID);

    //  terminate glfw
    glfwTerminate();
    return 0;
}
