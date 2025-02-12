## Description
Visualize different fractals using keyboard controls. The program supports three fractals: Sierpinski Triangle, Levy C Curve, and a Fractal Tree. Iteration depth can be adjusted dynamically, and information about the current fractal and depth is displayed in the console.

## Platform and Compiler (instructions apply for the following because this is what I used)
- **Operating System**: Fedora Linux (the Graphics Machines)
- **Compiler**: clang x86_64

## Compilation Instructions
1. Unzip the project folder.
2. Open a terminal or command prompt.
3. Navigate to the project folder:
   ```sh
   cd Assignment_1
   ```
After extracting the project, the directory structure will look like this:
``` 
│-- 453-skeleton/  
│-- assets/  
│-- thirdparty/ 
│-- README.md  
│-- CMakeLists.txt  
```

4. Create a `build` folder:
   ```sh
   mkdir build
   ```
5. Navigate to the `build` folder:
   ```sh
   cd build
   ```
6. Run the build command :
   
   ```sh
   cmake ..
   cmake --build .
   ```
7. Execute the compiled program as needed.
   ```sh
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

