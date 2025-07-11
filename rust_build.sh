#!/bin/bash

# Rust UAV Optimizer Build Script

echo "Building Rust UAV Flight Path Optimizer..."

# Create output directory
mkdir -p output

# Build the Rust project
cargo build --release

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Rust build completed successfully!"
    
    # Copy the binary to root directory for easy access
    cp target/release/uav_optimizer ./uav_optimizer_rust
    
    echo "Running Rust UAV Optimizer..."
    ./uav_optimizer_rust
else
    echo "Rust build failed!"
    exit 1
fi