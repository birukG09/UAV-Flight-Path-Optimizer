#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <iomanip>
#include "include/MapParser.h"
#include "include/Terrain.h"
#include "include/Drone.h"
#include "include/Optimizer.h"

// ANSI color codes for green terminal output
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define BRIGHT_GREEN "\033[1;32m"
#define DARK_GREEN "\033[2;32m"

void printUsage() {
    std::cout << "Usage: ./uav_optimizer [map_file] [start_x] [start_y] [end_x] [end_y]\n";
    std::cout << "Example: ./uav_optimizer maps/sample_map.txt 0 0 9 9\n";
    std::cout << "If no arguments provided, default sample will be used.\n";
}

void displayResults(const std::vector<Point>& path, const Terrain& terrain, 
                   const Drone& drone, double totalTime, double totalEnergy) {
    std::cout << GREEN << "\n=== UAV Flight Path Optimization Results ===" << RESET << "\n";
    std::cout << BRIGHT_GREEN << "Algorithm: A* Pathfinding" << RESET << "\n";
    std::cout << GREEN << "Computation Time: " << std::fixed << std::setprecision(3) << totalTime << " seconds" << RESET << "\n";
    std::cout << GREEN << "Total Energy Used: " << std::fixed << std::setprecision(2) << totalEnergy << " units" << RESET << "\n";
    std::cout << GREEN << "Path Length: " << path.size() << " steps" << RESET << "\n";
    std::cout << BRIGHT_GREEN << "Success Rate: " << (path.empty() ? "0%" : "100%") << RESET << "\n\n";
    
    if (!path.empty()) {
        std::cout << GREEN << "=== Flight Path Visualization ===" << RESET << "\n";
        terrain.visualizePath(path);
        std::cout << GREEN << "\n=== Legend ===" << RESET << "\n";
        std::cout << GREEN << "S = Start Point" << RESET << "\n";
        std::cout << GREEN << "D = Destination" << RESET << "\n";
        std::cout << GREEN << "O = Obstacle" << RESET << "\n";
        std::cout << GREEN << "^ = Hill (High Cost)" << RESET << "\n";
        std::cout << GREEN << "W = Wind Zone" << RESET << "\n";
        std::cout << GREEN << "* = Flight Path" << RESET << "\n";
        std::cout << GREEN << ". = Normal Terrain" << RESET << "\n";
    } else {
        std::cout << GREEN << "No path found! Target may be unreachable." << RESET << "\n";
    }
}

void savePerformanceLog(const std::vector<Point>& path, double totalTime, 
                       double totalEnergy, const std::string& filename) {
    std::ofstream logFile(filename);
    if (logFile.is_open()) {
        logFile << "timestamp,algorithm,path_length,computation_time,energy_used,success\n";
        
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        logFile << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S")
                << ",A*," << path.size() << "," << totalTime << "," 
                << totalEnergy << "," << (path.empty() ? "false" : "true") << "\n";
        
        logFile.close();
        std::cout << GREEN << "Performance log saved to: " << filename << RESET << "\n";
    } else {
        std::cout << GREEN << "Warning: Could not save performance log." << RESET << "\n";
    }
}

int main(int argc, char* argv[]) {
    std::cout << BRIGHT_GREEN << "=== UAV Flight Path Optimizer (AI-based) ===" << RESET << "\n";
    std::cout << GREEN << "Simulating intelligent drone navigation with obstacle avoidance" << RESET << "\n\n";
    
    // Default parameters
    std::string mapFile = "maps/sample_map.txt";
    Point start(0, 0);
    Point end(9, 9);
    
    // Parse command line arguments
    if (argc == 6) {
        mapFile = argv[1];
        start.x = std::stoi(argv[2]);
        start.y = std::stoi(argv[3]);
        end.x = std::stoi(argv[4]);
        end.y = std::stoi(argv[5]);
    } else if (argc != 1) {
        printUsage();
        return 1;
    }
    
    try {
        // Initialize components
        MapParser parser;
        Terrain terrain = parser.loadMap(mapFile);
        
        // Validate start and end points
        if (!terrain.isValidPosition(start) || !terrain.isValidPosition(end)) {
            std::cerr << "Error: Start or end position is invalid or blocked!\n";
            return 1;
        }
        
        if (terrain.isObstacle(start) || terrain.isObstacle(end)) {
            std::cerr << "Error: Start or end position is an obstacle!\n";
            return 1;
        }
        
        // Initialize drone and optimizer
        Drone drone(start, 1000.0); // 1000 energy units
        Optimizer optimizer(terrain);
        
        std::cout << GREEN << "Map loaded: " << mapFile << RESET << "\n";
        std::cout << GREEN << "Grid size: " << terrain.getWidth() << "x" << terrain.getHeight() << RESET << "\n";
        std::cout << GREEN << "Start: (" << start.x << ", " << start.y << ")" << RESET << "\n";
        std::cout << GREEN << "End: (" << end.x << ", " << end.y << ")" << RESET << "\n";
        std::cout << BRIGHT_GREEN << "Computing optimal path..." << RESET << "\n\n";
        
        // Measure computation time
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Find optimal path using A* algorithm
        std::vector<Point> path = optimizer.findPath(start, end);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        double totalTime = duration.count() / 1000.0;
        
        // Calculate total energy consumption
        double totalEnergy = 0.0;
        if (!path.empty()) {
            for (size_t i = 0; i < path.size(); i++) {
                totalEnergy += terrain.getMovementCost(path[i]);
            }
        }
        
        // Display results
        displayResults(path, terrain, drone, totalTime, totalEnergy);
        
        // Save performance log
        savePerformanceLog(path, totalTime, totalEnergy, "output/path_log.csv");
        
        // Performance validation
        if (totalTime > 2.0) {
            std::cout << GREEN << "\nWarning: Computation time exceeded 2 seconds threshold!" << RESET << "\n";
        }
        
        if (totalEnergy > drone.getMaxEnergy() * 0.85) {
            std::cout << GREEN << "\nWarning: Energy consumption exceeded 85% threshold!" << RESET << "\n";
        }
        
        std::cout << BRIGHT_GREEN << "\nSimulation completed successfully!" << RESET << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
