# Industrial Simulation - Circle Position Control

Enrico Piacenti S5636699

This assignment focuses on simulating an industrial scenario with two processes, A and B, each having its own window. Process A provides a user interface to control the position of a simplified circle and a print button to capture a snapshot of the current shared memory. The circle's position can be controlled using the arrow keys on the keyboard. Whenever the circle's position changes, a bitmap image is copied to the shared memory.

Process B displays the real-time position of the circle's center by analyzing the shared memory. All snapshots are saved in the directory `/out` and are sequentially numbered starting from 0.

## Installation and Execution
----------------------
To compile the simulation, a shell script is provided for easy execution:

$ ./compile.sh


Before running, make sure you have installed the *libbitmap* library, which is required for proper functionality. If you haven't installed it yet, follow these steps:

1. Download the source code from the [libbitmap GitHub repository](https://github.com/draekko/libbitmap.git) and save it in your file system.
2. Navigate to the root directory of the downloaded repository and run the configuration command: `./configure`. This may take some time and display messages about the checked features.
3. Compile the package using `make`.
4. Install the programs, data files, and documentation with `make install`.
5. Verify the successful installation by checking the presence of the `libbmp.so` shared library in `/usr/local/lib`.
6. To enable proper compilation of programs using the *libbitmap* library, add the following line at the end of your `.bashrc` file:

export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"


Once the setup is complete, you can run the simulation by using the following commands:

$ bash run_script.sh



## User Guide
----------------------
After following the instructions to compile and run the code, the simulation will be displayed with two windows: one for the moving image simulation and one for the position trace of the image's center.

1. Moving Image Simulation:
   - The first window shows a spot that can be moved using the arrow keys. This simulates the movement of a video camera.
   - The position of the spot represents the position of the circle in the industrial scenario.

2. Position Trace:
   - The second window displays a point that represents the center of the image from the first window.
   - As the spot is moved in the first window, the position of the center point in the second window will change accordingly.

3. Capturing Snapshots:
   - To capture a snapshot of the current image, press the arrow keys to move the spot and position it as desired.
   - Alternatively, you can click the "P" button using your mouse.
   - Each snapshot is saved as a .bmp file in the "out" directory.
   - Snapshots are sequentially numbered, starting from 0, to facilitate organization and reference.

4. Stopping the Simulation:
   - To stop the simulation, press "Ctrl + C" in the terminal.
   - This will terminate the program and close all open windows.



