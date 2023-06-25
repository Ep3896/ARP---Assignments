# Assignment 3: Advanced Remote Processing

Enrico Piacenti S5636699

This assignment introduces two new features - client and server connections - to facilitate communication between two similar applications running on different machines within a network. When launching the application, you will be prompted to choose one of three execution modes: normal (similar to assignment 2), server, or client. In server mode, the application receives input from another client application, while in client mode, it sends keyboard input to a server application.

## Installation and Execution

To compile the simulation, use the provided shell script:
$ bash run_script.sh

Ensure you have the *libbitmap* library installed, which is necessary for this application. If you haven't installed it, follow these steps:
1. Download the source code from the libbitmap GitHub repository: https://github.com/draekko/libbitmap.git and save it in your file system.
2. Navigate to the root directory of the downloaded repository and run the configuration command: ./configure. This may take a while and will display messages indicating the features being checked.
3. Execute make to compile the package.
4. Run make install to install the programs, data files, and documentation.
5. After installation, verify that the libbmp.so shared library is correctly installed in /usr/local/lib.
6. To compile programs using the libbitmap library, add the following line at the end of your .bashrc file:
export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"


## User Guide

After following the instructions to run the code, you will be presented with three program modes to choose from: normal, server, or client.

- Normal mode: The application runs as described in the previous assignment.
- Server mode: The application acts as a server and receives input from a client application running on a different machine. You will be prompted to enter the address and port of the companion application for communication. The server displays the received input on its ncurses window.
- Client mode: The application runs normally, as in Assignment 2, while also sending keyboard input to a server application on a different machine. You need to provide the address and port of the server application. The client transmits its keyboard inputs to the server for processing.

## Functionality Overview

For a detailed explanation of the program's basic functionality, refer to the readme of the second assignment: https://github.com/Ep3896/ARP---Assignments/tree/main/ARP-Piacenti-Second-Assignment .

Based on the selected mode, the program operates differently:

- In "normal" mode, process A reads input from the keyboard and displays it on its ncurses window. It also sends the input to process B through shared memory. Process B receives the input from shared memory and displays it on its ncurses window. The master process launches both process A and B, managing the shared memory and semaphores for inter-process communication.

- In "server" mode, process A acts as a server and waits for connections from a client application running in "client" mode. Once connected, process A receives input from the client and displays it on its ncurses window. The input is also sent to process B using shared memory. Process B receives the input from shared memory and displays it on its ncurses window.

- In "client" mode, process A acts as a client and establishes a connection with a server application running in "server" mode. Process A sends keyboard input to the server, which displays it on its ncurses window. Process B continues to receive input from shared memory and display it on its ncurses window, similar to the "normal" and "server" modes.


