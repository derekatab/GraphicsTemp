// Derek Atabayev
// CPSC 453 Assignment 1
// UCID: 30177060
// Implemented Assignment and Bonus 1 (IMGui)

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Window.h"
#include "AssetPath.h"
#include <functional>			   // added this for std::function
#include <glm/gtx/string_cast.hpp> // this is for printing glm::vec3 types, which I needed during the debugging

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// --- Three Fractal Generating Functions ---
void generateSierpinskiTriangle(CPU_Geometry &cpuGeom, int depth)
{
	// remove all existing vertices and colours from the CPU geometry (the VAO)
	cpuGeom.verts.clear();
	cpuGeom.cols.clear();

	// recursively generate a Sierpinski using a lambda function
	// I prefer lambda function because it has access to the outer scope, and it is a cleaner way to write recursive functions
	std::function<void(glm::vec3, glm::vec3, glm::vec3, int)> generate =
		[&](glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int depth)
	{
		if (depth == 0) // Base case: Depth is zero, draw a triangle
		{
			cpuGeom.verts.push_back(p1);
			cpuGeom.verts.push_back(p2);
			cpuGeom.verts.push_back(p3); // push all three vertices

			// Deterministic color based on vertex positions
			glm::vec3 color = glm::vec3((p1.x + 1.0f) / 2.0f, (p1.y + 1.0f) / 2.0f, 0.5f); // Color based on point position
			// all three vertices of a particular triangle will have the same color

			cpuGeom.cols.push_back(color);
			cpuGeom.cols.push_back(color);
			cpuGeom.cols.push_back(color);
		}
		// recursive case: divide into three smaller triangles
		else
		{
			// these are the midpoints of each side
			glm::vec3 mid1 = (p1 + p2) / 2.0f;
			glm::vec3 mid2 = (p2 + p3) / 2.0f;
			glm::vec3 mid3 = (p1 + p3) / 2.0f;

			// recursively call the generate function on the three smaller triangles
			generate(p1, mid1, mid3, depth - 1);
			generate(mid1, p2, mid2, depth - 1);
			generate(mid3, mid2, p3, depth - 1);
		}
	};

	// this is the initial call to the generate function with the main triangle
	generate(glm::vec3(-0.5f, -0.5f, 0.f), glm::vec3(0.5f, -0.5f, 0.f), glm::vec3(0.f, 0.5f, 0.f), depth);
}

void generateLevyCurve(CPU_Geometry &cpuGeom, int depth)
{ // for the c levy curve fractal
	cpuGeom.verts.clear();
	cpuGeom.cols.clear();

	std::function<void(glm::vec3, glm::vec3, int, float, float)> generate =
		[&](glm::vec3 p1, glm::vec3 p2, int depth, float t1, float t2)
	{
		if (depth == 0)
		{
			// the vector holds all points making the curve, so we push the two points
			// in the base case, there are only two
			cpuGeom.verts.push_back(p1);
			cpuGeom.verts.push_back(p2);

			// Gradient color calculation proceeds based on t1 and t2, which are the t values (a t value is a value between 0 and 1 used for interpolation)
			glm::vec3 color1 = glm::mix(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), t1); // Red to Green
			glm::vec3 color2 = glm::mix(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), t2);

			cpuGeom.cols.push_back(color1);
			cpuGeom.cols.push_back(color2); // added the two colours of line segment
		}
		else
		{
			glm::vec3 mid = (p1 + p2) / 2.0f; // take the mid point where the next line will be drawn
			glm::vec3 dir = p2 - p1;		  // find a directional vector
			glm::vec3 perp = glm::vec3(-dir.y, dir.x, 0.0f);
			// this is the perpendicular vector to the direction vector, which is used to find the next point
			// we do this by switching the x and y values and negating one of them
			mid += glm::normalize(perp) * glm::length(dir) * 0.5f;
			// as defined, we translate the mid point by half the length of the directional vector in the perpendicular direction
			float midT = (t1 + t2) / 2.0f;
			// the t value of the mid point is the average of the two t values of the end points, again this is just an interpolation parameter

			generate(p1, mid, depth - 1, t1, midT); // recursively generate two more curves from the mid point
			generate(mid, p2, depth - 1, midT, t2);
		}
	};

	// we use full interpolation for the first call, so the t values are 0 and 1
	// interpolation allows us to make the gradient of the line segment
	generate(glm::vec3(-0.5f, 0.0f, 0.f), glm::vec3(0.5f, 0.0f, 0.f), depth, 0.0f, 1.0f);
}

void generateTree(CPU_Geometry &cpuGeom, int depth)
{
	cpuGeom.verts.clear();
	cpuGeom.cols.clear();

	std::function<void(glm::vec3, glm::vec3, int)> generate =
		[&](glm::vec3 start, glm::vec3 end, int currentDepth)
	{
		// use a ternary operator to determine the color based on the depth, as described in the assignment
		glm::vec3 color = (currentDepth <= 3) ? glm::vec3(0.4f, 0.3f, 0.2f) : glm::vec3(0.13f, 0.55f, 0.13f);
		// according to Google, the colours are "darker desaturated brown" and "forest green"

		cpuGeom.verts.push_back(start); // add two endpoints and draw a line in between them
		cpuGeom.verts.push_back(end);
		cpuGeom.cols.push_back(color); // these line segments share the same colour
		cpuGeom.cols.push_back(color);

		if (currentDepth < depth) // recursive case
		{
			glm::vec3 dir = end - start;			 // finding a directional vector
			float length = glm::length(dir);		 // finding the length of the directional vector aka "length of the branch"
			glm::vec3 unitDir = glm::normalize(dir); // normalizing the directional vector so that we can scale it

			const float angle = glm::radians(25.7f); // branch angle as defined in the assignment
			const float cosA = cos(angle);			 // precalculate the cos and sin of the angle
			const float sinA = sin(angle);

			glm::vec3 branch1End = end + unitDir * (length * 0.5f); // the first branch is just a scaled version of the original branch (straight ahead)
			glm::vec3 midpoint = (start + end) * 0.5f;				// the midpoint of the branch

			// we need to rotate to get the other two branches
			glm::vec3 branch2Dir = glm::vec3(unitDir.x * cosA - unitDir.y * sinA, unitDir.x * sinA + unitDir.y * cosA, 0.0f) * (length * 0.5f);	 // this is like + 25.7 degrees from vertical, find its endpoint
			glm::vec3 branch3Dir = glm::vec3(unitDir.x * cosA + unitDir.y * sinA, -unitDir.x * sinA + unitDir.y * cosA, 0.0f) * (length * 0.5f); // this is like - 25.7 degrees from vertical, find its endpoint

			generate(end, branch1End, currentDepth + 1);				 // first branch
			generate(midpoint, midpoint + branch2Dir, currentDepth + 1); // second branch
			generate(midpoint, midpoint + branch3Dir, currentDepth + 1); // third branch, the endpoint is the midpoint plus the rotated vector
		}
	};
	generate(glm::vec3(0.0f, -0.8f, 0.0f), glm::vec3(0.0f, -0.3f, 0.0f), 0);
}

// --- Different Fractals ---

// Fractal enum
enum FractalTypes
{
	SierpinskiTriangle,
	LevyCurve,
	Tree
}; // this is to reduce the confusion with the switch function

// Create an array of fractal names which match the enum values for printing std::cout
const char *fractalNames[] = {
	"Sierpinski Triangle",
	"Levy Curve",
	"Tree"};

// Fractal configuration
// use a struct to have the parameters for each fractal (max iteration, current iteration, and drawing mode)
struct FractalConfig
{
	int maxIteration;
	int currentIteration;
	GLenum drawingMode;
};

// Declare currentFractal as a global variable, otherwise it can't be assigned by the keyCallback override function
// the default value will be Sierpinski (1)
FractalTypes currentFractal = SierpinskiTriangle;

// assign the appropriate parameters, we won't exceed the requirements
FractalConfig fractalConfigs[] = {
	{6, 0, GL_TRIANGLES}, // Sierpinski Triangle
	{12, 0, GL_LINES},	  // Levy Curve
	{10, 0, GL_LINES}	  // Tree
};

void updateFractal(CPU_Geometry &cGeom, GPU_Geometry &gGeom)
{															// now we update the fractal based on the current type/iteration (whatever needs to be updated)
	FractalConfig &config = fractalConfigs[currentFractal]; // find the entry in the struct array

	// what we do here is call the relevant method this updates the CPU geometry data container before sending it to the GPU

	switch (currentFractal)
	{
	case SierpinskiTriangle:
		generateSierpinskiTriangle(cGeom, config.currentIteration);
		break;
	case LevyCurve:
		generateLevyCurve(cGeom, config.currentIteration);
		break;

	case Tree:
		generateTree(cGeom, config.currentIteration);
		break;
	}
	gGeom.setVerts(cGeom.verts); // Update the geometry from and pass it to the wrapper gGeom to send to GPU
	gGeom.setCols(cGeom.cols);	 // same thing for colours
}

// --- Callbacks ---

class MyCallbacks : public CallbackInterface
{

public:
	MyCallbacks(ShaderProgram &shader, CPU_Geometry &cGeom, GPU_Geometry &gGeom) : shader(shader), cGeom(cGeom), gGeom(gGeom) {}

	virtual void keyCallback(int key, int scancode, int action, int mods) override
	{							  // respond to key presses
		if (action == GLFW_PRESS) // was a key pressed?
		{
			if (key >= GLFW_KEY_1 && key <= GLFW_KEY_3) // yes, a key was pressed, but was it a number key?
			{
				currentFractal = static_cast<FractalTypes>(key - GLFW_KEY_1);		   // the enum of fractal types uses zero-based indexing
				updateFractal(cGeom, gGeom);										   // update the fractal based on the new type and current iteration
				std::cout << "Fractal: " << fractalNames[currentFractal] << std::endl; // print the name of the fractal
			}
			else if (key == GLFW_KEY_UP) // increase iteration depth
			{
				FractalConfig &config = fractalConfigs[currentFractal];
				config.currentIteration = std::min(config.currentIteration + 1, config.maxIteration); // increment iteration, at max clamp and do not proceed further
				updateFractal(cGeom, gGeom);														  // regenerate fractal
				std::cout << "Iteration: " << config.currentIteration << std::endl;
			}
			else if (key == GLFW_KEY_DOWN)
			{
				FractalConfig &config = fractalConfigs[currentFractal];
				config.currentIteration = std::max(config.currentIteration - 1, 0); // decrement iteration depth, at min clamp and do not proceed further
				updateFractal(cGeom, gGeom);
				std::cout << "Iteration: " << config.currentIteration << std::endl;
			}
			else
			{
				std::cout << "Invalid fractal selected" << std::endl;
			}
		}
	}

	// not implementing any other callbacks

private:
	ShaderProgram &shader;
	CPU_Geometry &cGeom;
	GPU_Geometry &gGeom; // add references so that we can update the geometry
};

class MyCallbacks2 : public CallbackInterface
{ // I didn't do Bonus 2 so this is not used

public:
	MyCallbacks2() {}

	virtual void keyCallback(int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_R && action == GLFW_PRESS)
		{
			std::cout << "called back" << std::endl;
		}
	}
};

int main()
{
	Log::debug("Starting main");

	// WINDOW
	glfwInit();													// MUST call this first to set up environment (There is a terminate pair after the loop)
	Window window(800, 800, "CPSC 453 Assignment 1: Fractals"); // Can set callbacks at construction if desired

	// GLDebug::enable(); // ON Submission you may comments this out to avoid unnecessary prints to the console

	// --- Setup of ImGUI ---
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();		  // create a new context

	ImGui::StyleColorsDark(); // set the style to dark

	ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true); // initialize the imgui for the window
	ImGui_ImplOpenGL3_Init("#version 460");						// initialize the imgui for the opengl version which the shaders have also been using

	// SHADERS
	ShaderProgram shader(
		AssetPath::Instance()->Get("shaders/basic.vert"),
		AssetPath::Instance()->Get("shaders/basic.frag")); // Render pipeline we will use (You can use more than one!)

	// GEOMETRY
	CPU_Geometry cGeom; // Just a collection of vectors with geometry information
	GPU_Geometry gGeom; // A wrapper managing VBOs, presumably

	// CALLBACKS
	std::shared_ptr<MyCallbacks> callback_ptr = std::make_shared<MyCallbacks>(shader, cGeom, gGeom); // Class To capture input events
	// std::shared_ptr<MyCallbacks2> callback2_ptr = std::make_shared<MyCallbacks2>(); // not used
	window.setCallbacks(callback_ptr); // when a callback occurs, the window shall call the callback_ptr

	updateFractal(cGeom, gGeom); // initialize the initial fractal geometry (default: Sierpinski)

	// RENDER LOOP
	while (!window.shouldClose())
	{
		shader.use(); // Use "this" shader to render
		gGeom.bind(); // USe "this" VAO (Geometry) on render call

		glEnable(GL_FRAMEBUFFER_SRGB); // Expect Colour to be encoded in sRGB standard (as opposed to RGB)
		// https://www.viewsonic.com/library/creative-work/srgb-vs-adobe-rgb-which-one-to-use/
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear render screen (all zero) and depth (all max depth)

		// --- Start a new ImGui frame ---
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuiIO& io = ImGui::GetIO(); // retrieve a new IO for configuration

		if (!io.WantCaptureKeyboard && !io.WantCaptureMouse)
		{ // Only grab events if ImGui doesn't need them, because it handles them as a priority
			glfwPollEvents();
		}

		// Create a window called "Control Fractals" --- for user to manage
		ImGui::Begin("Control Fractals", nullptr, ImGuiWindowFlags_AlwaysAutoResize); // make the window resizable and larger
		ImGui::SetWindowSize(ImVec2(500, 300)); // Larger size for the window

		// Add a combo box to select the fractal type
		if (ImGui::Combo("Fractal Type", reinterpret_cast<int *>(&currentFractal), fractalNames, IM_ARRAYSIZE(fractalNames)))
		{
			updateFractal(cGeom, gGeom); // update the fractal based on the new type and current iteration
		}
		FractalConfig &config = fractalConfigs[currentFractal]; // find the entry in the struct array

		// Add a slider so that we can change the iteration depth
		if (ImGui::SliderInt("Iteration Depth", &config.currentIteration, 0, config.maxIteration))
		{
			updateFractal(cGeom, gGeom); // update the fractal based on the new type and current iteration
		}

		ImGui::End(); // End the window

		shader.use(); // Use "this" shader to render
		gGeom.bind(); // Use "this" VAO (Geometry) on render call

		glEnable(GL_FRAMEBUFFER_SRGB); // Expect Colour to be encoded in sRGB standard (as opposed to RGB)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear render screen (all zero) and depth (all max depth)
		glDrawArrays(fractalConfigs[currentFractal].drawingMode, 0, static_cast<GLsizei>(cGeom.verts.size()));
		// this is the draw call, works by referencing the struct for drawing mode and the size of the vertices, which is casted to GLsizei because it is an unsigned int
		glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for the imgui

		// End ImGui frame
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // this renders the imgui data before the swap buffers

		window.swapBuffers(); // Swap the buffers while displaying the previous
	}

	// good practice is to clean up the ImGui context
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate(); // Clean up GLFW
	return 0;
}
