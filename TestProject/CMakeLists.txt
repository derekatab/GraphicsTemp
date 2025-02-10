cmake_minimum_required(VERSION 3.10)
project(TestProject) # Project name

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# include directories available
include_directories(${CMAKE_SOURCE_DIR}/include)

# we also need glfw, glew, glm
include_directories("C:/Program Files/OpenGL/glfw/include")
include_directories("C:/Program Files/OpenGL/glew/include")
include_directories("C:/Program Files/OpenGL/glm")

# link directories
link_directories("C:/Program Files/OpenGL/glfw/lib-vc2022") # grab the .lib files
link_directories("C:/Program Files/OpenGL/glew/lib/Release/x64")

# create executable
add_executable(TestProject src/main.cpp)


# link all the required libraries
target_link_libraries(TestProject glfw3 glew32s opengl32)

