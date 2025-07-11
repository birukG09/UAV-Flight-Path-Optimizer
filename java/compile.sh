#!/bin/bash

# UAV Optimizer GUI Compilation Script
echo "=== UAV Optimizer GUI Build Script ==="

# Create build directories
mkdir -p build/classes build/jar

# Clean previous builds
rm -rf build/classes/*
rm -rf build/jar/*

# Compile Java source files
echo "Compiling Java source files..."
javac -d build/classes -sourcepath src/main/java src/main/java/com/uav/optimizer/*.java

if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    
    # Create JAR file
    echo "Creating JAR file..."
    cd build/classes
    jar cfe ../jar/uav-optimizer-gui.jar com.uav.optimizer.UAVOptimizerGUI com/uav/optimizer/*.class
    cd ../..
    
    echo "JAR file created: build/jar/uav-optimizer-gui.jar"
    echo "Run with: java -jar build/jar/uav-optimizer-gui.jar"
else
    echo "Compilation failed!"
    exit 1
fi