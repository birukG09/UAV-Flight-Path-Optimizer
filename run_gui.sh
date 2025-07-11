#!/bin/bash

# UAV Optimizer GUI Launcher
echo "Starting UAV Flight Path Optimizer GUI..."

# Build the Java GUI if not already built
if [ ! -f "java/build/jar/uav-optimizer-gui.jar" ]; then
    echo "Building Java GUI..."
    cd java
    mkdir -p build/classes build/jar
    javac -d build/classes -sourcepath src/main/java src/main/java/com/uav/optimizer/*.java
    if [ $? -eq 0 ]; then
        cd build/classes
        jar cfe ../jar/uav-optimizer-gui.jar com.uav.optimizer.UAVOptimizerGUI com/uav/optimizer/*.class
        cd ../..
        echo "Java GUI built successfully!"
    else
        echo "Failed to build Java GUI"
        exit 1
    fi
    cd ..
fi

# Make sure C++ backend is built
if [ ! -f "uav_optimizer" ]; then
    echo "Building C++ backend..."
    make clean && make all
    if [ $? -ne 0 ]; then
        echo "Failed to build C++ backend"
        exit 1
    fi
fi

# Run the GUI
echo "Launching GUI..."
cd java
java -jar build/jar/uav-optimizer-gui.jar