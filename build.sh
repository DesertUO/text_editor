#!/bin/zsh

clear

echo "Creating build directory..."
mkdir -p build

echo "Configuring the project with cmake..."
cmake -S . -B build

echo "Building the project..."
cmake --build build

echo "Running the project"
./build/TEditor
