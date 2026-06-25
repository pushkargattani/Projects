#!/bin/bash
# Compile main.cpp inside src folder and run

g++ -std=c++17 -O2 -o SocialNet src/main.cpp

if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo "Running program with sample_input.txt..."
    ./SocialNet < sample_input.txt
else
    echo "Compilation failed!"
fi
