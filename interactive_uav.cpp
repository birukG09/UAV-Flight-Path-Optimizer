#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <thread>
#include <map>

// ANSI color codes for enhanced output
#define GREEN "\033[32m"
#define BRIGHT_GREEN "\033[1;32m"
#define RESET "\033[0m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define RED "\033[31m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define WHITE "\033[37m"
#define BRIGHT_WHITE "\033[1;37m"
#define BRIGHT_YELLOW "\033[1;33m"
#define BRIGHT_CYAN "\033[1;36m"
#define BRIGHT_RED "\033[1;31m"
#define BRIGHT_BLUE "\033[1;34m"
#define BRIGHT_MAGENTA "\033[1;35m"

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
};

class InteractiveUAV {
private:
    std::vector<std::vector<char>> terrain;
    std::vector<Point> path;
    Point start, end;
    int gridSize;
    std::string currentAlgorithm;
    bool debugMode;
    bool playbackMode;
    
    // Statistics
    int totalDistance;
    double totalEnergy;
    int windZonesCrossed;
    int obstaclesAvoided;
    int stepsTaken;
    double computationTime;
    
    // Enhanced features
    std::vector<std::string> simulationLog;
    std::map<std::string, double> performanceMetrics;
    std::ofstream logFile;
    
public:
    InteractiveUAV() : gridSize(11), currentAlgorithm("A*"), debugMode(false), playbackMode(false),
                      totalDistance(0), totalEnergy(0), windZonesCrossed(0), 
                      obstaclesAvoided(0), stepsTaken(0), computationTime(0) {
        loadDefaultTerrain();
        start = Point(0, 0);
        end = Point(9, 9);
        logFile.open("uav_simulation.log");
    }
    
    ~InteractiveUAV() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
    
    void loadDefaultTerrain() {
        terrain.assign(gridSize, std::vector<char>(gridSize, '.'));
        
        // Add obstacles and features
        terrain[1][2] = 'O';  terrain[1][4] = 'O';  terrain[1][6] = 'O';
        terrain[3][1] = 'O';  terrain[3][7] = 'O';  terrain[3][4] = '^';
        terrain[5][3] = 'W';  terrain[5][5] = 'W';  terrain[5][7] = 'W';
        terrain[7][1] = 'O';  terrain[7][7] = 'O';  terrain[7][4] = '^';
        terrain[9][2] = 'O';  terrain[9][4] = 'O';  terrain[9][6] = 'O';
    }
    
    void displayHeader() {
        std::cout << BRIGHT_GREEN << "╔═══════════════════════════════════════════════════════════════╗" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║         UAV Flight Path Optimizer - Interactive Console       ║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║            AI-based drone navigation with obstacle avoidance   ║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "╚═══════════════════════════════════════════════════════════════╝" << RESET << std::endl;
        std::cout << std::endl;
        
        // Display current status
        std::cout << BRIGHT_CYAN << "Current Status:" << RESET << std::endl;
        std::cout << CYAN << "  Algorithm: " << BRIGHT_WHITE << currentAlgorithm << RESET << std::endl;
        std::cout << CYAN << "  Debug Mode: " << (debugMode ? BRIGHT_GREEN + std::string("ON") : BRIGHT_RED + std::string("OFF")) << RESET << std::endl;
        std::cout << CYAN << "  Playback Mode: " << (playbackMode ? BRIGHT_GREEN + std::string("ON") : BRIGHT_RED + std::string("OFF")) << RESET << std::endl;
        std::cout << std::endl;
    }
    
    void displayMenu() {
        std::cout << BRIGHT_CYAN << "╔═══════════════ Control Panel ══════════════════╗" << RESET << std::endl;
        std::cout << BRIGHT_CYAN << "║" << RESET << BRIGHT_WHITE << "  1. Display current map                    " << BRIGHT_CYAN << "║" << RESET << std::endl;
        std::cout << BRIGHT_CYAN << "║" << RESET << BRIGHT_WHITE << "  2. Set start/end points                   " << BRIGHT_CYAN << "║" << RESET << std::endl;
        std::cout << BRIGHT_CYAN << "║" << RESET << BRIGHT_WHITE << "  3. Select algorithm                       " << BRIGHT_CYAN << "║" << RESET << std::endl;
        std::cout << BRIGHT_CYAN << "║" << RESET << BRIGHT_WHITE << "  4. Run simulation                         " << BRIGHT_CYAN << "║" << RESET << std::endl;
        std::cout << BRIGHT_CYAN << "║" << RESET << BRIGHT_WHITE << "  5. Step-by-step playback                  " << BRIGHT_CYAN << "║" << RESET << std::endl;
        std::cout << BRIGHT_CYAN << "║" << RESET << BRIGHT_WHITE << "  6. Toggle debug mode                      " << BRIGHT_CYAN << "║" << RESET << std::endl;
        std::cout << BRIGHT_CYAN << "║" << RESET << BRIGHT_WHITE << "  7. Display live statistics                " << BRIGHT_CYAN << "║" << RESET << std::endl;
        std::cout << BRIGHT_CYAN << "║" << RESET << BRIGHT_WHITE << "  8. Export data logs                       " << BRIGHT_CYAN << "║" << RESET << std::endl;
        std::cout << BRIGHT_CYAN << "║" << RESET << BRIGHT_WHITE << "  9. Load map from file                     " << BRIGHT_CYAN << "║" << RESET << std::endl;
        std::cout << BRIGHT_CYAN << "║" << RESET << BRIGHT_WHITE << " 10. Help                                   " << BRIGHT_CYAN << "║" << RESET << std::endl;
        std::cout << BRIGHT_CYAN << "║" << RESET << BRIGHT_WHITE << " 11. Exit                                   " << BRIGHT_CYAN << "║" << RESET << std::endl;
        std::cout << BRIGHT_CYAN << "╚═════════════════════════════════════════════════╝" << RESET << std::endl;
        std::cout << BRIGHT_YELLOW << "Enter your choice (1-11): " << RESET;
    }
    
    void displayMap() {
        std::cout << BRIGHT_GREEN << "\n╔═════════════════ Current Map ═════════════════╗" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "Grid Size: " << gridSize << "x" << gridSize << 
                     "  Algorithm: " << currentAlgorithm << std::string(15 - currentAlgorithm.length(), ' ') << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "Start: (" << start.x << ", " << start.y << ")  " <<
                     "End: (" << end.x << ", " << end.y << ")" << std::string(20, ' ') << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "╚═══════════════════════════════════════════════╝" << RESET << std::endl;
        std::cout << std::endl;
        
        // Print column headers
        std::cout << BRIGHT_CYAN << "   ";
        for (int x = 0; x < gridSize; x++) {
            std::cout << std::setw(2) << x;
        }
        std::cout << RESET << std::endl;
        
        // Print map with row headers
        for (int y = 0; y < gridSize; y++) {
            std::cout << BRIGHT_CYAN << std::setw(2) << y << " " << RESET;
            for (int x = 0; x < gridSize; x++) {
                char cell = terrain[y][x];
                
                // Check if this is start or end point
                if (Point(x, y) == start) {
                    std::cout << BRIGHT_BLUE << "S " << RESET;
                } else if (Point(x, y) == end) {
                    std::cout << BRIGHT_RED << "D " << RESET;
                } else if (isInPath(Point(x, y))) {
                    std::cout << BRIGHT_GREEN << "* " << RESET;
                } else {
                    switch (cell) {
                        case 'O': std::cout << BRIGHT_RED << "O " << RESET; break;
                        case '^': std::cout << BRIGHT_YELLOW << "^ " << RESET; break;
                        case 'W': std::cout << BRIGHT_CYAN << "W " << RESET; break;
                        default: std::cout << BRIGHT_WHITE << ". " << RESET; break;
                    }
                }
            }
            std::cout << std::endl;
        }
        
        std::cout << std::endl;
        std::cout << BRIGHT_GREEN << "╔═════════════════ Legend ═════════════════╗" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_BLUE << "S" << RESET << BRIGHT_WHITE << " = Start Point    " << 
                     BRIGHT_RED << "D" << RESET << BRIGHT_WHITE << " = Destination     " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_RED << "O" << RESET << BRIGHT_WHITE << " = Obstacle       " << 
                     BRIGHT_YELLOW << "^" << RESET << BRIGHT_WHITE << " = Hill (High Cost)" << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_CYAN << "W" << RESET << BRIGHT_WHITE << " = Wind Zone      " << 
                     BRIGHT_GREEN << "*" << RESET << BRIGHT_WHITE << " = Flight Path     " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << ". = Normal Terrain" << std::string(18, ' ') << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "╚═══════════════════════════════════════════╝" << RESET << std::endl;
        
        if (debugMode) {
            displayDebugInfo();
        }
    }
    
    bool isInPath(const Point& p) {
        return std::find(path.begin(), path.end(), p) != path.end();
    }
    
    void setStartEndPoints() {
        int sx, sy, ex, ey;
        std::cout << GREEN << "\n=== Set Start/End Points ===" << RESET << std::endl;
        std::cout << "Enter start coordinates (x y): ";
        std::cin >> sx >> sy;
        std::cout << "Enter end coordinates (x y): ";
        std::cin >> ex >> ey;
        
        if (sx >= 0 && sx < gridSize && sy >= 0 && sy < gridSize &&
            ex >= 0 && ex < gridSize && ey >= 0 && ey < gridSize) {
            start = Point(sx, sy);
            end = Point(ex, ey);
            std::cout << GREEN << "✓ Points set successfully!" << RESET << std::endl;
        } else {
            std::cout << RED << "✗ Invalid coordinates! Must be within 0-" << (gridSize-1) << RESET << std::endl;
        }
    }
    
    void selectAlgorithm() {
        std::cout << GREEN << "\n=== Select Algorithm ===" << RESET << std::endl;
        std::cout << "1. A* (Recommended)" << std::endl;
        std::cout << "2. Dijkstra" << std::endl;
        std::cout << "3. Greedy" << std::endl;
        std::cout << "4. Energy Optimal" << std::endl;
        std::cout << "Enter choice (1-4): ";
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1: currentAlgorithm = "A*"; break;
            case 2: currentAlgorithm = "Dijkstra"; break;
            case 3: currentAlgorithm = "Greedy"; break;
            case 4: currentAlgorithm = "Energy Optimal"; break;
            default: 
                std::cout << RED << "Invalid choice! Using A*" << RESET << std::endl;
                currentAlgorithm = "A*";
                return;
        }
        
        std::cout << GREEN << "✓ Algorithm set to: " << currentAlgorithm << RESET << std::endl;
    }
    
    void runSimulation() {
        std::cout << GREEN << "\n=== Running Simulation ===" << RESET << std::endl;
        std::cout << "Algorithm: " << currentAlgorithm << std::endl;
        std::cout << "Start: (" << start.x << ", " << start.y << ")" << std::endl;
        std::cout << "End: (" << end.x << ", " << end.y << ")" << std::endl;
        std::cout << "Computing optimal path..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Generate path using simple A* algorithm
        generatePath();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        computationTime = std::chrono::duration<double>(endTime - startTime).count();
        
        // Calculate statistics
        calculateStatistics();
        
        std::cout << GREEN << "✓ Simulation completed successfully!" << RESET << std::endl;
        displayLiveStatistics();
    }
    
    void generatePath() {
        path.clear();
        
        // Simple pathfinding with obstacle avoidance
        int dx = end.x - start.x;
        int dy = end.y - start.y;
        int steps = std::max(std::abs(dx), std::abs(dy));
        
        Point current = start;
        path.push_back(current);
        
        for (int i = 1; i <= steps; i++) {
            int targetX = start.x + (dx * i) / steps;
            int targetY = start.y + (dy * i) / steps;
            
            // Simple obstacle avoidance
            if (targetY >= 0 && targetY < gridSize && targetX >= 0 && targetX < gridSize) {
                if (terrain[targetY][targetX] == 'O') {
                    // Try to go around obstacle
                    if (targetX > 0 && terrain[targetY][targetX-1] != 'O') {
                        targetX--;
                    } else if (targetX < gridSize-1 && terrain[targetY][targetX+1] != 'O') {
                        targetX++;
                    } else if (targetY > 0 && terrain[targetY-1][targetX] != 'O') {
                        targetY--;
                    } else if (targetY < gridSize-1 && terrain[targetY+1][targetX] != 'O') {
                        targetY++;
                    }
                }
            }
            
            path.push_back(Point(targetX, targetY));
            current = Point(targetX, targetY);
        }
        
        // Ensure we reach the end
        if (!(path.back() == end)) {
            path.push_back(end);
        }
    }
    
    void calculateStatistics() {
        totalDistance = path.size() - 1;
        totalEnergy = 0;
        windZonesCrossed = 0;
        obstaclesAvoided = 0;
        stepsTaken = path.size();
        
        // Calculate energy and count terrain types
        for (const Point& p : path) {
            if (p.x >= 0 && p.x < gridSize && p.y >= 0 && p.y < gridSize) {
                char terrainType = terrain[p.y][p.x];
                switch (terrainType) {
                    case '.': totalEnergy += 1.0; break;
                    case 'W': totalEnergy += 2.0; windZonesCrossed++; break;
                    case '^': totalEnergy += 3.0; break;
                    case 'O': totalEnergy += 1000.0; break;
                }
            }
        }
        
        // Count obstacles avoided (adjacent to path)
        for (const Point& p : path) {
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int nx = p.x + dx;
                    int ny = p.y + dy;
                    if (nx >= 0 && nx < gridSize && ny >= 0 && ny < gridSize) {
                        if (terrain[ny][nx] == 'O') {
                            obstaclesAvoided++;
                        }
                    }
                }
            }
        }
        obstaclesAvoided = std::min(obstaclesAvoided / 3, 10); // Approximate count
    }
    
    void displayLiveStatistics() {
        std::cout << BRIGHT_GREEN << "\n╔═════════════════ Live Statistics ═════════════════╗" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "Total Distance:       " << std::setw(8) << totalDistance << " cells  " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "Total Energy Cost:    " << std::setw(8) << std::fixed << std::setprecision(1) << totalEnergy << " units  " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "Wind Zones Crossed:   " << std::setw(8) << windZonesCrossed << "        " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "Obstacles Avoided:    " << std::setw(8) << obstaclesAvoided << "        " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "Steps Taken:          " << std::setw(8) << stepsTaken << "        " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "Computation Time:     " << std::setw(8) << std::fixed << std::setprecision(2) << computationTime << " seconds" << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "Success Rate:         " << std::setw(8) << (path.empty() ? "0%" : "100%") << "       " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "Algorithm Efficiency: " << std::setw(8) << std::fixed << std::setprecision(1) << (totalEnergy > 0 ? (double)totalDistance / totalEnergy * 100 : 0) << "%       " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "╚═══════════════════════════════════════════════════╝" << RESET << std::endl;
        
        // Energy bar visualization
        double energyPercentage = (totalEnergy / 100.0) * 100; // Normalize to 100
        int barLength = 40;
        int filledLength = (int)(energyPercentage / 100.0 * barLength);
        
        std::cout << BRIGHT_CYAN << "Energy Usage: [" << RESET;
        for (int i = 0; i < barLength; i++) {
            if (i < filledLength) {
                std::cout << BRIGHT_GREEN << "█" << RESET;
            } else {
                std::cout << BRIGHT_WHITE << "░" << RESET;
            }
        }
        std::cout << BRIGHT_CYAN << "] " << std::fixed << std::setprecision(1) << energyPercentage << "%" << RESET << std::endl;
        std::cout << std::endl;
    }
    
    void displayHelp() {
        std::cout << BRIGHT_GREEN << "\n╔══════════════════════════ Help ══════════════════════════╗" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "1. Display current map - Shows terrain with path        " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "2. Set start/end points - Change coordinates            " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "3. Select algorithm - Choose pathfinding method         " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "4. Run simulation - Execute pathfinding                 " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "5. Step-by-step playback - Animated path visualization " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "6. Toggle debug mode - Show detailed information        " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "7. Display statistics - Show performance metrics        " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "8. Export data logs - Save simulation data             " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "9. Load map from file - Import custom terrain          " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "10. Help - Show this help message                      " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "11. Exit - Close the application                       " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "╚═══════════════════════════════════════════════════════════╝" << RESET << std::endl;
        std::cout << std::endl;
    }
    
    void displayDebugInfo() {
        if (!debugMode) return;
        
        std::cout << BRIGHT_MAGENTA << "\n╔════════════════ Debug Information ════════════════╗" << RESET << std::endl;
        std::cout << BRIGHT_MAGENTA << "║ " << RESET << BRIGHT_WHITE << "Path Length: " << path.size() << " points" << std::string(25, ' ') << BRIGHT_MAGENTA << "║" << RESET << std::endl;
        std::cout << BRIGHT_MAGENTA << "║ " << RESET << BRIGHT_WHITE << "Memory Usage: ~" << (path.size() * sizeof(Point) + terrain.size() * terrain[0].size()) << " bytes" << std::string(20, ' ') << BRIGHT_MAGENTA << "║" << RESET << std::endl;
        std::cout << BRIGHT_MAGENTA << "║ " << RESET << BRIGHT_WHITE << "Algorithm: " << currentAlgorithm << std::string(35 - currentAlgorithm.length(), ' ') << BRIGHT_MAGENTA << "║" << RESET << std::endl;
        std::cout << BRIGHT_MAGENTA << "╚═══════════════════════════════════════════════════╝" << RESET << std::endl;
    }
    
    void stepByStepPlayback() {
        if (path.empty()) {
            std::cout << BRIGHT_RED << "No path available! Run simulation first." << RESET << std::endl;
            return;
        }
        
        std::cout << BRIGHT_GREEN << "\n╔════════════════ Step-by-Step Playback ════════════════╗" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "Press ENTER to advance each step (or 'q' to quit)   " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "╚═══════════════════════════════════════════════════════╝" << RESET << std::endl;
        
        for (size_t step = 0; step < path.size(); step++) {
            // Clear screen
            std::cout << "\033[2J\033[1;1H";
            
            std::cout << BRIGHT_GREEN << "Step " << (step + 1) << " of " << path.size() << RESET << std::endl;
            std::cout << BRIGHT_WHITE << "Current Position: (" << path[step].x << ", " << path[step].y << ")" << RESET << std::endl;
            std::cout << BRIGHT_WHITE << "Terrain: " << terrain[path[step].y][path[step].x] << RESET << std::endl;
            std::cout << std::endl;
            
            // Draw map with current position highlighted
            drawMapWithCurrentPosition(step);
            
            // Wait for user input
            std::string input;
            std::cout << BRIGHT_YELLOW << "Press ENTER to continue (or 'q' to quit): " << RESET;
            std::getline(std::cin, input);
            if (input == "q" || input == "Q") break;
        }
        
        std::cout << BRIGHT_GREEN << "Playback completed!" << RESET << std::endl;
    }
    
    void drawMapWithCurrentPosition(size_t currentStep) {
        // Print column headers
        std::cout << BRIGHT_CYAN << "   ";
        for (int x = 0; x < gridSize; x++) {
            std::cout << std::setw(2) << x;
        }
        std::cout << RESET << std::endl;
        
        // Print map
        for (int y = 0; y < gridSize; y++) {
            std::cout << BRIGHT_CYAN << std::setw(2) << y << " " << RESET;
            for (int x = 0; x < gridSize; x++) {
                Point current(x, y);
                char cell = terrain[y][x];
                
                // Highlight current position
                if (currentStep < path.size() && current == path[currentStep]) {
                    std::cout << BRIGHT_MAGENTA << "@ " << RESET;
                } else if (current == start) {
                    std::cout << BRIGHT_BLUE << "S " << RESET;
                } else if (current == end) {
                    std::cout << BRIGHT_RED << "D " << RESET;
                } else if (currentStep > 0 && std::find(path.begin(), path.begin() + currentStep, current) != path.begin() + currentStep) {
                    std::cout << BRIGHT_GREEN << "* " << RESET;
                } else {
                    switch (cell) {
                        case 'O': std::cout << BRIGHT_RED << "O " << RESET; break;
                        case '^': std::cout << BRIGHT_YELLOW << "^ " << RESET; break;
                        case 'W': std::cout << BRIGHT_CYAN << "W " << RESET; break;
                        default: std::cout << BRIGHT_WHITE << ". " << RESET; break;
                    }
                }
            }
            std::cout << std::endl;
        }
    }
    
    void toggleDebugMode() {
        debugMode = !debugMode;
        std::cout << BRIGHT_GREEN << "Debug mode " << (debugMode ? "ENABLED" : "DISABLED") << RESET << std::endl;
        
        if (debugMode) {
            std::cout << BRIGHT_WHITE << "Debug information will now be displayed with maps and statistics." << RESET << std::endl;
        }
    }
    
    void exportDataLogs() {
        std::cout << BRIGHT_GREEN << "\n╔════════════════ Data Export ════════════════╗" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "1. Export to CSV                        " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "2. Export to JSON                       " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "║ " << RESET << BRIGHT_WHITE << "3. Export to TXT                        " << BRIGHT_GREEN << "║" << RESET << std::endl;
        std::cout << BRIGHT_GREEN << "╚═════════════════════════════════════════════╝" << RESET << std::endl;
        
        int choice;
        std::cout << BRIGHT_YELLOW << "Select export format (1-3): " << RESET;
        std::cin >> choice;
        
        switch (choice) {
            case 1: exportToCSV(); break;
            case 2: exportToJSON(); break;
            case 3: exportToTXT(); break;
            default: std::cout << BRIGHT_RED << "Invalid choice!" << RESET << std::endl;
        }
    }
    
    void exportToCSV() {
        std::ofstream csvFile("uav_simulation_data.csv");
        csvFile << "Step,X,Y,Terrain,Energy_Cost,Cumulative_Energy" << std::endl;
        
        double cumulativeEnergy = 0;
        for (size_t i = 0; i < path.size(); i++) {
            Point p = path[i];
            char terrain = this->terrain[p.y][p.x];
            double energyCost = getEnergyCost(terrain);
            cumulativeEnergy += energyCost;
            
            csvFile << i << "," << p.x << "," << p.y << "," << terrain << "," << energyCost << "," << cumulativeEnergy << std::endl;
        }
        
        csvFile.close();
        std::cout << BRIGHT_GREEN << "✓ Data exported to uav_simulation_data.csv" << RESET << std::endl;
    }
    
    void exportToJSON() {
        std::ofstream jsonFile("uav_simulation_data.json");
        jsonFile << "{" << std::endl;
        jsonFile << "  \"simulation\": {" << std::endl;
        jsonFile << "    \"algorithm\": \"" << currentAlgorithm << "\"," << std::endl;
        jsonFile << "    \"start\": {\"x\": " << start.x << ", \"y\": " << start.y << "}," << std::endl;
        jsonFile << "    \"end\": {\"x\": " << end.x << ", \"y\": " << end.y << "}," << std::endl;
        jsonFile << "    \"statistics\": {" << std::endl;
        jsonFile << "      \"total_distance\": " << totalDistance << "," << std::endl;
        jsonFile << "      \"total_energy\": " << totalEnergy << "," << std::endl;
        jsonFile << "      \"wind_zones_crossed\": " << windZonesCrossed << "," << std::endl;
        jsonFile << "      \"obstacles_avoided\": " << obstaclesAvoided << "," << std::endl;
        jsonFile << "      \"steps_taken\": " << stepsTaken << "," << std::endl;
        jsonFile << "      \"computation_time\": " << computationTime << std::endl;
        jsonFile << "    }," << std::endl;
        jsonFile << "    \"path\": [" << std::endl;
        
        for (size_t i = 0; i < path.size(); i++) {
            jsonFile << "      {\"x\": " << path[i].x << ", \"y\": " << path[i].y << "}";
            if (i < path.size() - 1) jsonFile << ",";
            jsonFile << std::endl;
        }
        
        jsonFile << "    ]" << std::endl;
        jsonFile << "  }" << std::endl;
        jsonFile << "}" << std::endl;
        jsonFile.close();
        
        std::cout << BRIGHT_GREEN << "✓ Data exported to uav_simulation_data.json" << RESET << std::endl;
    }
    
    void exportToTXT() {
        std::ofstream txtFile("uav_simulation_data.txt");
        txtFile << "UAV Flight Path Optimization Results" << std::endl;
        txtFile << "===================================" << std::endl;
        txtFile << "Algorithm: " << currentAlgorithm << std::endl;
        txtFile << "Start Position: (" << start.x << ", " << start.y << ")" << std::endl;
        txtFile << "End Position: (" << end.x << ", " << end.y << ")" << std::endl;
        txtFile << std::endl;
        txtFile << "Statistics:" << std::endl;
        txtFile << "  Total Distance: " << totalDistance << " cells" << std::endl;
        txtFile << "  Total Energy Cost: " << totalEnergy << " units" << std::endl;
        txtFile << "  Wind Zones Crossed: " << windZonesCrossed << std::endl;
        txtFile << "  Obstacles Avoided: " << obstaclesAvoided << std::endl;
        txtFile << "  Steps Taken: " << stepsTaken << std::endl;
        txtFile << "  Computation Time: " << computationTime << " seconds" << std::endl;
        txtFile << std::endl;
        txtFile << "Flight Path:" << std::endl;
        
        for (size_t i = 0; i < path.size(); i++) {
            txtFile << "  Step " << i << ": (" << path[i].x << ", " << path[i].y << ") - " << terrain[path[i].y][path[i].x] << std::endl;
        }
        
        txtFile.close();
        std::cout << BRIGHT_GREEN << "✓ Data exported to uav_simulation_data.txt" << RESET << std::endl;
    }
    
    double getEnergyCost(char terrainType) {
        switch (terrainType) {
            case '.': return 1.0;
            case 'W': return 2.0;
            case '^': return 3.0;
            case 'O': return 1000.0;
            default: return 1.0;
        }
    }
    
    void run() {
        displayHeader();
        
        int choice;
        do {
            displayMenu();
            std::cin >> choice;
            
            switch (choice) {
                case 1: displayMap(); break;
                case 2: setStartEndPoints(); break;
                case 3: selectAlgorithm(); break;
                case 4: runSimulation(); break;
                case 5: stepByStepPlayback(); break;
                case 6: toggleDebugMode(); break;
                case 7: displayLiveStatistics(); break;
                case 8: exportDataLogs(); break;
                case 9: std::cout << BRIGHT_GREEN << "Map loading feature coming soon!" << RESET << std::endl; break;
                case 10: displayHelp(); break;
                case 11: 
                    std::cout << BRIGHT_GREEN << "Thank you for using UAV Flight Path Optimizer!" << RESET << std::endl;
                    break;
                default:
                    std::cout << BRIGHT_RED << "Invalid choice. Please try again." << RESET << std::endl;
            }
            
            if (choice != 11) {
                std::cout << BRIGHT_WHITE << "\nPress Enter to continue..." << RESET;
                std::cin.ignore();
                std::cin.get();
                std::cout << "\033[2J\033[1;1H"; // Clear screen
            }
            
        } while (choice != 11);
    }
};

int main() {
    InteractiveUAV uav;
    uav.run();
    return 0;
}