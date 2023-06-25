#!/bin/bash

[ ! -d bin ] && mkdir -p bin | echo "bin directory created"
[ ! -d log ] && mkdir -p log | echo "log directory created"

echo "Compiling..."
gcc src/processA.c -lncurses -lbmp -lm -o bin/processA
gcc src/processB.c -lncurses -lbmp -lm -o bin/processB
gcc src/master.c -o bin/master

echo "Starting program..."
./bin/master
