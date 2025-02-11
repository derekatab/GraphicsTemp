## Description
Visualize different fractals using keyboard controls. The program supports three fractals: Sierpinski Triangle, Levy C Curve, and a Fractal Tree. Iteration depth can be adjusted dynamically, and information about the current fractal and depth is displayed in the console.

## Platform and Compiler (instructions apply for the following because this is what I used)
- **Operating System**: Windows 10
- **Compiler**: Microsoft Visual C++ (MSVC) using CMake

## Compilation Instructions
1. Unzip the project folder.
2. Open a terminal or command prompt and navigate (`cd`) to the `build` folder.
3. Run the following command to compile the project:
   ```
   cmake --build .
   ```
   (or an equivalent command for your compiler)
4. Navigate to the `debug` folder:
   ```
   cd debug
   ```
5. Run the executable:
   ```
   ./453-skeleton
   ```

## Controls
- **Press 1**: Render Sierpinski Triangle
- **Press 2**: Render Levy C Curve
- **Press 3**: Render Fractal Tree
- **Up Arrow**: Increase iteration depth
- **Down Arrow**: Decrease iteration depth

**Note:** While ImGui is included in the project, mouse input has not been integrated, so only keyboard controls are functional.

For real-time updates, check the console output, which displays the current fractal and iteration depth.

