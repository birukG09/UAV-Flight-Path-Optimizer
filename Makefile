# UAV Flight Path Optimizer Makefile
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude
SRCDIR = src
INCDIR = include
OBJDIR = obj
OUTDIR = output

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
TARGET = uav_optimizer

# Default target
all: setup $(TARGET)

# Create necessary directories
setup:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OUTDIR)
	@mkdir -p maps

# Build target
$(TARGET): $(OBJECTS) main.cpp
	$(CXX) $(CXXFLAGS) -o $@ main.cpp $(OBJECTS)

# Build object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJDIR) $(TARGET)

# Clean all generated files
clean-all: clean
	rm -rf $(OUTDIR)

# Run with sample map
run-sample: $(TARGET)
	./$(TARGET) maps/sample_map.txt 0 0 10 10

# Run with complex map
run-complex: $(TARGET)
	./$(TARGET) maps/complex_map.txt 0 0 20 12

# Run with default parameters
run: $(TARGET)
	./$(TARGET)

# Run Rust version
run-rust: rust-build
	./target/release/uav_optimizer

# Build Rust version
rust-build:
	cargo build --release

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Release build
release: CXXFLAGS += -O3 -DNDEBUG
release: $(TARGET)

# Install dependencies (none needed for this project)
install:
	@echo "No external dependencies required"

# Help
help:
	@echo "Available targets:"
	@echo "  all         - Build the C++ project"
	@echo "  clean       - Clean build files"
	@echo "  clean-all   - Clean all generated files"
	@echo "  run         - Run C++ version with default parameters"
	@echo "  run-sample  - Run C++ version with sample map"
	@echo "  run-complex - Run C++ version with complex map"
	@echo "  run-rust    - Run Rust version with default parameters"
	@echo "  rust-build  - Build Rust version"
	@echo "  debug       - Build debug version"
	@echo "  release     - Build optimized version"
	@echo "  help        - Show this help message"

# Phony targets
.PHONY: all setup clean clean-all run run-sample run-complex run-rust rust-build debug release install help