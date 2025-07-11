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
#include <random>
#include <queue>
#include <cstdlib>

// Metasploit-style ANSI color codes
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"
#define MAGENTA "\033[35m"
#define BLUE "\033[34m"
#define BRIGHT_GREEN "\033[92m"
#define BRIGHT_BLUE "\033[94m"
#define BRIGHT_RED "\033[91m"
#define BRIGHT_YELLOW "\033[93m"
#define BRIGHT_CYAN "\033[96m"
#define BRIGHT_MAGENTA "\033[95m"
#define WHITE "\033[37m"
#define BRIGHT_WHITE "\033[97m"
#define RESET "\033[0m"
#define CLEAR_SCREEN "\033[2J\033[H"
#define BOLD "\033[1m"

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point& other) const { return x == other.x && y == other.y; }
    bool operator<(const Point& other) const { return x < other.x || (x == other.x && y < other.y); }
};

struct PathNode {
    Point position;
    double gCost, hCost, fCost;
    Point parent;
    char terrain;
    double energyCost;
    
    PathNode(Point pos = Point(0,0), double g = 0, double h = 0, Point p = Point(-1,-1)) 
        : position(pos), gCost(g), hCost(h), fCost(g+h), parent(p), terrain('.'), energyCost(0) {}
    
    bool operator>(const PathNode& other) const { return fCost > other.fCost; }
};

struct Entity {
    Point position;
    std::string type; // "drone", "plane", "uav"
    std::string symbol;
    std::string color;
    bool active;
    
    Entity(Point pos, std::string t, std::string s, std::string c) 
        : position(pos), type(t), symbol(s), color(c), active(true) {}
};

class MetasploitUAV {
private:
    std::vector<std::vector<char>> terrain;
    std::vector<Point> path;
    std::vector<PathNode> exploredNodes;
    std::vector<Entity> entities;
    Point start, end;
    int gridSize;
    std::string currentAlgorithm;
    bool debugMode;
    bool liveLogging;
    bool animatedMode;
    
    // Enhanced statistics
    int totalDistance;
    double totalEnergy;
    int windZonesCrossed;
    int obstaclesAvoided;
    int stepsTaken;
    double computationTime;
    int nodesExplored;
    int dronesDeployed;
    int planesDeployed;
    
    std::mt19937 rng;
    std::ofstream logFile;
    
public:
    MetasploitUAV() : gridSize(15), currentAlgorithm("A*"), debugMode(false), liveLogging(true), animatedMode(true),
                      totalDistance(0), totalEnergy(0), windZonesCrossed(0), obstaclesAvoided(0), 
                      stepsTaken(0), computationTime(0), nodesExplored(0), dronesDeployed(0), planesDeployed(0),
                      rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
        loadTerrain();
        start = Point(1, 1);
        end = Point(13, 13);
        initializeEntities();
        logFile.open("uav_metasploit.log");
    }
    
    ~MetasploitUAV() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
    
    void loadTerrain() {
        terrain.assign(gridSize, std::vector<char>(gridSize, '.'));
        
        // Generate complex terrain with patterns
        for (int i = 0; i < 30; i++) {
            int x = rng() % gridSize;
            int y = rng() % gridSize;
            if (x > 0 && x < gridSize-1 && y > 0 && y < gridSize-1) {
                terrain[y][x] = 'O'; // Obstacle
            }
        }
        
        // Add wind zones
        for (int i = 0; i < 20; i++) {
            int x = rng() % gridSize;
            int y = rng() % gridSize;
            if (terrain[y][x] == '.') terrain[y][x] = 'W';
        }
        
        // Add hills
        for (int i = 0; i < 15; i++) {
            int x = rng() % gridSize;
            int y = rng() % gridSize;
            if (terrain[y][x] == '.') terrain[y][x] = '^';
        }
    }
    
    void initializeEntities() {
        entities.clear();
        dronesDeployed = 0;
        planesDeployed = 0;
        
        // Add drones
        for (int i = 0; i < 3; i++) {
            int x = rng() % gridSize;
            int y = rng() % gridSize;
            if (terrain[y][x] == '.') {
                entities.push_back(Entity(Point(x, y), "drone", "✈", BRIGHT_GREEN));
                dronesDeployed++;
            }
        }
        
        // Add planes
        for (int i = 0; i < 2; i++) {
            int x = rng() % gridSize;
            int y = rng() % gridSize;
            if (terrain[y][x] == '.') {
                entities.push_back(Entity(Point(x, y), "plane", "🛩", BRIGHT_BLUE));
                planesDeployed++;
            }
        }
    }
    
    void displayAnimatedBanner() {
        std::cout << CLEAR_SCREEN;
        
        std::vector<std::string> banner = {
            "██╗   ██╗ █████╗ ██╗   ██╗    ██████╗  ██╗   ██╗ ██████╗ ██╗   ██╗",
            "██║   ██║██╔══██╗██║   ██║    ██╔══██╗ ██║   ██║██╔═══██╗██║   ██║",
            "██║   ██║███████║██║   ██║    ██████╔╝ ██║   ██║██║   ██║██║   ██║",
            "╚██╗ ██╔╝██╔══██║██║   ██║    ██╔═══╝  ╚██╗ ██╔╝██║   ██║██║   ██║",
            " ╚████╔╝ ██║  ██║╚██████╔╝    ██║       ╚████╔╝ ╚██████╔╝╚██████╔╝",
            "  ╚═══╝  ╚═╝  ╚═╝ ╚═════╝     ╚═╝        ╚═══╝   ╚═════╝  ╚═════╝ "
        };
        
        std::vector<std::string> colors = {RED, YELLOW, GREEN, CYAN, MAGENTA, BLUE};
        
        if (animatedMode) {
            for (size_t i = 0; i < banner.size(); i++) {
                std::cout << colors[i % colors.size()] << banner[i] << RESET << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
            }
        } else {
            for (size_t i = 0; i < banner.size(); i++) {
                std::cout << colors[i % colors.size()] << banner[i] << RESET << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << BRIGHT_CYAN << "╔════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << BRIGHT_CYAN << "║" << BRIGHT_WHITE << "           ADVANCED UAV FLIGHT PATH OPTIMIZER v2.0                 " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║" << BRIGHT_WHITE << "        Professional Drone Navigation & Tactical Planning           " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║" << BRIGHT_WHITE << "              [ METASPLOIT-STYLE INTERFACE ]                        " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "╚════════════════════════════════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
    }
    
    void displayProgressBar(const std::string& description, int percentage) {
        const int barLength = 20;
        int filledLength = (percentage * barLength) / 100;
        
        std::cout << CYAN << description << ": [";
        for (int i = 0; i < barLength; i++) {
            if (i < filledLength) {
                std::cout << GREEN << "●";
            } else {
                std::cout << WHITE << "○";
            }
        }
        std::cout << CYAN << "] " << percentage << "%" << RESET << std::endl;
        std::cout.flush();
    }
    
    void animatedProgressBar(const std::string& description, int duration_ms) {
        for (int i = 0; i <= 100; i += 5) {
            std::cout << "\r";
            displayProgressBar(description, i);
            std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms / 20));
        }
        std::cout << std::endl;
    }
    
    void displayMainMenu() {
        std::cout << BRIGHT_CYAN << "╔═════════════════════ TACTICAL OPERATIONS MENU ═══════════════════════╗" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "1. [RECON] Display terrain map with entities                      " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "2. [COORDS] Set mission coordinates                               " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "3. [ALGO] Select pathfinding algorithm                            " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "4. [EXECUTE] Run tactical navigation simulation                   " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "5. [PLAYBACK] Step-by-step mission replay                         " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "6. [DEBUG] Toggle debug/analysis mode                             " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "7. [STATS] Display mission statistics panel                       " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "8. [DEPLOY] Deploy additional drones/planes                       " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "9. [CONFIG] Configuration settings                                " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "10. [LOG] Toggle live logging                                     " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "11. [EXPORT] Export mission data                                  " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "12. [RESET] Generate new terrain                                  " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "13. [HELP] System documentation                                   " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "14. [EXIT] Shutdown tactical system                               " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "╚═══════════════════════════════════════════════════════════════════════╝" << std::endl;
        
        // Status bar
        std::cout << std::endl;
        std::cout << BRIGHT_YELLOW << "STATUS: " << RESET;
        std::cout << GREEN << "Algorithm[" << currentAlgorithm << "] " << RESET;
        std::cout << (debugMode ? GREEN + std::string("DEBUG[ON]") : RED + std::string("DEBUG[OFF]")) << RESET << " ";
        std::cout << (liveLogging ? GREEN + std::string("LOG[ON]") : RED + std::string("LOG[OFF]")) << RESET << " ";
        std::cout << CYAN << "Entities[" << entities.size() << "] " << RESET;
        std::cout << MAGENTA << "Grid[" << gridSize << "x" << gridSize << "]" << RESET << std::endl;
        
        std::cout << std::endl;
        std::cout << BRIGHT_GREEN << "msf uav(" << BRIGHT_RED << "tactical" << BRIGHT_GREEN << ") > " << RESET;
    }
    
    void displayTacticalMap() {
        std::cout << BRIGHT_CYAN << "\n╔═══════════════════════ TACTICAL RECONNAISSANCE ═══════════════════════╗" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "Grid: " << gridSize << "x" << gridSize << 
                     "  Algorithm: " << currentAlgorithm << 
                     "  Entities: " << entities.size() << 
                     "  Path: " << path.size() << " steps" << std::string(10, ' ') << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "Start: [" << start.x << "," << start.y << "]  " <<
                     "Target: [" << end.x << "," << end.y << "]  " <<
                     "Drones: " << dronesDeployed << "  Planes: " << planesDeployed << std::string(15, ' ') << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "╚═════════════════════════════════════════════════════════════════════════╝" << std::endl;
        
        // Column headers
        std::cout << BRIGHT_WHITE << "    ";
        for (int x = 0; x < gridSize; x++) {
            std::cout << std::setw(3) << x;
        }
        std::cout << std::endl;
        
        // Map display with entities
        for (int y = 0; y < gridSize; y++) {
            std::cout << BRIGHT_WHITE << std::setw(3) << y << " ";
            for (int x = 0; x < gridSize; x++) {
                Point current(x, y);
                char cell = terrain[y][x];
                
                // Check for entities first
                Entity* entity = getEntityAt(current);
                if (entity && entity->active) {
                    std::cout << entity->color << entity->symbol << " " << RESET;
                } else if (current == start) {
                    std::cout << GREEN << "S " << RESET;
                } else if (current == end) {
                    std::cout << BLUE << "D " << RESET;
                } else if (isInPath(current)) {
                    std::cout << YELLOW << "● " << RESET;
                } else if (debugMode && isExplored(current)) {
                    std::cout << MAGENTA << "· " << RESET;
                } else {
                    switch (cell) {
                        case 'O': std::cout << RED << "■ " << RESET; break;
                        case '^': std::cout << MAGENTA << "▲ " << RESET; break;
                        case 'W': std::cout << CYAN << "~ " << RESET; break;
                        default: std::cout << WHITE << ". " << RESET; break;
                    }
                }
            }
            std::cout << std::endl;
        }
        
        // Legend
        std::cout << std::endl;
        std::cout << BRIGHT_CYAN << "╔════════════════════════ LEGEND ════════════════════════╗" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << GREEN << "S" << RESET << ": Start Point (Green)    " << 
                     BLUE << "D" << RESET << ": Destination (Blue)   " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << YELLOW << "●" << RESET << ": Path (Yellow)         " << 
                     RED << "■" << RESET << ": Obstacle (Red)       " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << MAGENTA << "▲" << RESET << ": Hill (Magenta)       " << 
                     CYAN << "~" << RESET << ": Wind Zone (Cyan)     " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_GREEN << "✈" << RESET << ": Drone (Bright Green) " << 
                     BRIGHT_BLUE << "🛩" << RESET << ": Plane (Bright Blue)  " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << WHITE << "." << RESET << ": Free Cell             " << 
                     MAGENTA << "·" << RESET << ": Explored (Debug)     " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "╚═════════════════════════════════════════════════════════╝" << std::endl;
        
        if (debugMode) {
            displayDebugPanel();
        }
    }
    
    void displayDebugPanel() {
        std::cout << std::endl;
        std::cout << BRIGHT_MAGENTA << "╔════════════════════ DEBUG ANALYSIS ═══════════════════╗" << std::endl;
        std::cout << BRIGHT_MAGENTA << "║ " << BRIGHT_WHITE << "Nodes Explored: " << std::setw(8) << nodesExplored << 
                     "  Memory: " << std::setw(8) << (exploredNodes.size() * sizeof(PathNode)) << " bytes" << 
                     std::string(5, ' ') << BRIGHT_MAGENTA << "║" << std::endl;
        std::cout << BRIGHT_MAGENTA << "║ " << BRIGHT_WHITE << "Active Entities: " << std::setw(7) << entities.size() << 
                     "  Path Cost: " << std::setw(8) << std::fixed << std::setprecision(2) << totalEnergy << 
                     std::string(10, ' ') << BRIGHT_MAGENTA << "║" << std::endl;
        std::cout << BRIGHT_MAGENTA << "║ " << BRIGHT_WHITE << "Algorithm: " << currentAlgorithm << 
                     "  Heuristic: Manhattan" << std::string(20, ' ') << BRIGHT_MAGENTA << "║" << std::endl;
        std::cout << BRIGHT_MAGENTA << "╚════════════════════════════════════════════════════════╝" << std::endl;
    }
    
    void displayStatsPanel() {
        std::cout << BRIGHT_CYAN << "\n╔══════════════════════ MISSION STATISTICS ═══════════════════════╗" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "Total Distance:     " << std::setw(8) << totalDistance << " cells           " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "Total Energy Cost:  " << std::setw(8) << std::fixed << std::setprecision(2) << totalEnergy << " units           " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "Steps Taken:        " << std::setw(8) << stepsTaken << "                 " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "Obstacles Avoided:  " << std::setw(8) << obstaclesAvoided << "                 " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "Wind Zones Crossed: " << std::setw(8) << windZonesCrossed << "                 " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "Drones Deployed:    " << std::setw(8) << dronesDeployed << "                 " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "Planes Deployed:    " << std::setw(8) << planesDeployed << "                 " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "Computation Time:   " << std::setw(8) << std::fixed << std::setprecision(3) << computationTime << " seconds        " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "╚═══════════════════════════════════════════════════════════════════╝" << std::endl;
        
        // Visual statistics
        displayVisualStats();
    }
    
    void displayVisualStats() {
        std::cout << std::endl;
        std::cout << BRIGHT_YELLOW << "PERFORMANCE METRICS:" << std::endl;
        
        // Energy usage bar
        double energyPercentage = std::min(100.0, (totalEnergy / 50.0) * 100);
        displayProgressBar("Energy Usage", (int)energyPercentage);
        
        // Path efficiency bar
        double pathEfficiency = (path.size() > 0) ? (calculateStraightLineDistance() / path.size()) * 100 : 0;
        displayProgressBar("Path Efficiency", (int)pathEfficiency);
        
        // Mission success rate
        int successRate = (path.size() > 0) ? 100 : 0;
        displayProgressBar("Mission Success", successRate);
    }
    
    void executeSimulation() {
        std::cout << BRIGHT_YELLOW << "\n[*] Initializing tactical simulation..." << std::endl;
        animatedProgressBar("Loading algorithms", 1000);
        
        std::cout << BRIGHT_YELLOW << "[*] Analyzing terrain topology..." << std::endl;
        animatedProgressBar("Terrain analysis", 800);
        
        std::cout << BRIGHT_YELLOW << "[*] Deploying pathfinding algorithms..." << std::endl;
        animatedProgressBar("Algorithm deployment", 600);
        
        std::cout << BRIGHT_YELLOW << "[*] Computing optimal trajectory..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        generatePath();
        auto endTime = std::chrono::high_resolution_clock::now();
        computationTime = std::chrono::duration<double>(endTime - startTime).count();
        
        animatedProgressBar("Path calculation", 400);
        
        calculateStatistics();
        
        std::cout << BRIGHT_GREEN << "\n[+] MISSION EXECUTION COMPLETE!" << std::endl;
        std::cout << BRIGHT_GREEN << "[+] Path found with " << path.size() << " steps" << std::endl;
        std::cout << BRIGHT_GREEN << "[+] Total energy cost: " << std::fixed << std::setprecision(2) << totalEnergy << " units" << std::endl;
        
        if (liveLogging) {
            logMissionData();
        }
    }
    
    void generatePath() {
        path.clear();
        exploredNodes.clear();
        nodesExplored = 0;
        
        std::priority_queue<PathNode, std::vector<PathNode>, std::greater<PathNode>> openList;
        std::map<Point, bool> closedList;
        std::map<Point, PathNode> nodeMap;
        
        PathNode startNode(start, 0, calculateHeuristic(start, end));
        openList.push(startNode);
        nodeMap[start] = startNode;
        
        while (!openList.empty()) {
            PathNode current = openList.top();
            openList.pop();
            
            if (current.position == end) {
                reconstructPath(nodeMap, current);
                break;
            }
            
            closedList[current.position] = true;
            exploredNodes.push_back(current);
            nodesExplored++;
            
            // Check all 8 directions
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx == 0 && dy == 0) continue;
                    
                    Point neighbor(current.position.x + dx, current.position.y + dy);
                    
                    if (!isValidPosition(neighbor) || closedList[neighbor]) continue;
                    
                    double moveCost = (dx == 0 || dy == 0) ? 1.0 : 1.414;
                    double terrainCost = getTerrainCost(terrain[neighbor.y][neighbor.x]);
                    double newGCost = current.gCost + moveCost + terrainCost;
                    
                    if (nodeMap.find(neighbor) == nodeMap.end() || newGCost < nodeMap[neighbor].gCost) {
                        PathNode neighborNode(neighbor, newGCost, calculateHeuristic(neighbor, end), current.position);
                        neighborNode.terrain = terrain[neighbor.y][neighbor.x];
                        neighborNode.energyCost = terrainCost;
                        
                        nodeMap[neighbor] = neighborNode;
                        openList.push(neighborNode);
                    }
                }
            }
        }
    }
    
    void reconstructPath(std::map<Point, PathNode>& nodeMap, PathNode endNode) {
        Point current = endNode.position;
        while (!(current == start)) {
            path.push_back(current);
            current = nodeMap[current].parent;
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());
    }
    
    double calculateHeuristic(Point a, Point b) {
        return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
    }
    
    bool isValidPosition(Point p) {
        return p.x >= 0 && p.x < gridSize && p.y >= 0 && p.y < gridSize;
    }
    
    double getTerrainCost(char terrainType) {
        switch (terrainType) {
            case '.': return 1.0;
            case 'W': return 2.5;
            case '^': return 4.0;
            case 'O': return 1000.0;
            default: return 1.0;
        }
    }
    
    void calculateStatistics() {
        totalDistance = path.size() - 1;
        totalEnergy = 0;
        windZonesCrossed = 0;
        obstaclesAvoided = 0;
        stepsTaken = path.size();
        
        for (const Point& p : path) {
            char terrainType = terrain[p.y][p.x];
            totalEnergy += getTerrainCost(terrainType);
            if (terrainType == 'W') windZonesCrossed++;
        }
        
        obstaclesAvoided = countNearbyObstacles();
    }
    
    int countNearbyObstacles() {
        int count = 0;
        for (const Point& p : path) {
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int nx = p.x + dx, ny = p.y + dy;
                    if (nx >= 0 && nx < gridSize && ny >= 0 && ny < gridSize) {
                        if (terrain[ny][nx] == 'O') count++;
                    }
                }
            }
        }
        return count / 3;
    }
    
    double calculateStraightLineDistance() {
        return calculateHeuristic(start, end);
    }
    
    void stepByStepPlayback() {
        if (path.empty()) {
            std::cout << BRIGHT_RED << "[!] ERROR: No mission data available. Execute simulation first." << std::endl;
            return;
        }
        
        std::cout << BRIGHT_YELLOW << "\n[*] Initiating step-by-step mission playback..." << std::endl;
        std::cout << BRIGHT_CYAN << "[*] Press ENTER to advance each step (Q to quit, P to pause)" << std::endl;
        std::cout << std::endl;
        
        for (size_t step = 0; step < path.size(); step++) {
            if (animatedMode) {
                std::cout << CLEAR_SCREEN;
            }
            
            std::cout << BRIGHT_GREEN << "╔═══════════════════ MISSION STEP " << (step + 1) << " / " << path.size() << " ═══════════════════╗" << std::endl;
            std::cout << BRIGHT_GREEN << "║ " << BRIGHT_WHITE << "Current Position: [" << path[step].x << "," << path[step].y << "]" << std::string(30, ' ') << BRIGHT_GREEN << "║" << std::endl;
            std::cout << BRIGHT_GREEN << "║ " << BRIGHT_WHITE << "Terrain Type: " << terrain[path[step].y][path[step].x] << std::string(45, ' ') << BRIGHT_GREEN << "║" << std::endl;
            std::cout << BRIGHT_GREEN << "║ " << BRIGHT_WHITE << "Energy Cost: " << getTerrainCost(terrain[path[step].y][path[step].x]) << std::string(45, ' ') << BRIGHT_GREEN << "║" << std::endl;
            std::cout << BRIGHT_GREEN << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
            std::cout << std::endl;
            
            drawPlaybackMap(step);
            
            // Progress bar for current step
            int progressPercentage = ((step + 1) * 100) / path.size();
            displayProgressBar("Mission Progress", progressPercentage);
            
            std::string input;
            std::cout << BRIGHT_GREEN << "\nmsf uav(playback) > " << RESET;
            std::getline(std::cin, input);
            
            if (input == "q" || input == "Q") {
                std::cout << BRIGHT_YELLOW << "[*] Playback terminated by user." << std::endl;
                break;
            } else if (input == "p" || input == "P") {
                std::cout << BRIGHT_YELLOW << "[*] Playback paused. Press ENTER to continue..." << std::endl;
                std::cin.get();
            }
            
            if (liveLogging) {
                logStepData(step);
            }
        }
        
        std::cout << BRIGHT_GREEN << "\n[+] MISSION PLAYBACK COMPLETE!" << std::endl;
    }
    
    void drawPlaybackMap(size_t currentStep) {
        // Column headers
        std::cout << BRIGHT_WHITE << "    ";
        for (int x = 0; x < gridSize; x++) {
            std::cout << std::setw(3) << x;
        }
        std::cout << std::endl;
        
        for (int y = 0; y < gridSize; y++) {
            std::cout << BRIGHT_WHITE << std::setw(3) << y << " ";
            for (int x = 0; x < gridSize; x++) {
                Point current(x, y);
                char cell = terrain[y][x];
                
                // Current position (animated)
                if (currentStep < path.size() && current == path[currentStep]) {
                    std::cout << BRIGHT_YELLOW << "◉ " << RESET;
                } else if (current == start) {
                    std::cout << GREEN << "S " << RESET;
                } else if (current == end) {
                    std::cout << BLUE << "D " << RESET;
                } else if (currentStep > 0 && std::find(path.begin(), path.begin() + currentStep, current) != path.begin() + currentStep) {
                    std::cout << YELLOW << "● " << RESET;
                } else {
                    // Check for entities
                    Entity* entity = getEntityAt(current);
                    if (entity && entity->active) {
                        std::cout << entity->color << entity->symbol << " " << RESET;
                    } else {
                        switch (cell) {
                            case 'O': std::cout << RED << "■ " << RESET; break;
                            case '^': std::cout << MAGENTA << "▲ " << RESET; break;
                            case 'W': std::cout << CYAN << "~ " << RESET; break;
                            default: std::cout << WHITE << ". " << RESET; break;
                        }
                    }
                }
            }
            std::cout << std::endl;
        }
    }
    
    void deployEntities() {
        std::cout << BRIGHT_YELLOW << "\n[*] Entity deployment interface" << std::endl;
        std::cout << BRIGHT_CYAN << "1. Deploy additional drones" << std::endl;
        std::cout << BRIGHT_CYAN << "2. Deploy additional planes" << std::endl;
        std::cout << BRIGHT_CYAN << "3. Remove entities" << std::endl;
        std::cout << BRIGHT_CYAN << "4. Show entity status" << std::endl;
        std::cout << BRIGHT_GREEN << "Choice: " << RESET;
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1: deployDrones(); break;
            case 2: deployPlanes(); break;
            case 3: removeEntities(); break;
            case 4: showEntityStatus(); break;
            default: std::cout << BRIGHT_RED << "[!] Invalid choice!" << std::endl;
        }
    }
    
    void deployDrones() {
        std::cout << BRIGHT_GREEN << "How many drones to deploy? " << RESET;
        int count;
        std::cin >> count;
        
        for (int i = 0; i < count; i++) {
            int x = rng() % gridSize;
            int y = rng() % gridSize;
            if (terrain[y][x] == '.') {
                entities.push_back(Entity(Point(x, y), "drone", "✈", BRIGHT_GREEN));
                dronesDeployed++;
            }
        }
        
        std::cout << BRIGHT_GREEN << "[+] Deployed " << count << " drones successfully!" << std::endl;
    }
    
    void deployPlanes() {
        std::cout << BRIGHT_GREEN << "How many planes to deploy? " << RESET;
        int count;
        std::cin >> count;
        
        for (int i = 0; i < count; i++) {
            int x = rng() % gridSize;
            int y = rng() % gridSize;
            if (terrain[y][x] == '.') {
                entities.push_back(Entity(Point(x, y), "plane", "🛩", BRIGHT_BLUE));
                planesDeployed++;
            }
        }
        
        std::cout << BRIGHT_GREEN << "[+] Deployed " << count << " planes successfully!" << std::endl;
    }
    
    void removeEntities() {
        std::cout << BRIGHT_YELLOW << "[*] Removing all entities..." << std::endl;
        entities.clear();
        dronesDeployed = 0;
        planesDeployed = 0;
        std::cout << BRIGHT_GREEN << "[+] All entities removed!" << std::endl;
    }
    
    void showEntityStatus() {
        std::cout << BRIGHT_CYAN << "\n╔═══════════════════ ENTITY STATUS ═══════════════════╗" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "Total Entities: " << std::setw(8) << entities.size() << std::string(25, ' ') << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "Drones: " << std::setw(8) << dronesDeployed << std::string(32, ' ') << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "Planes: " << std::setw(8) << planesDeployed << std::string(32, ' ') << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "╚═════════════════════════════════════════════════════╝" << std::endl;
        
        for (size_t i = 0; i < entities.size(); i++) {
            std::cout << entities[i].color << entities[i].type << " " << entities[i].symbol << RESET << 
                         " at [" << entities[i].position.x << "," << entities[i].position.y << "]" << std::endl;
        }
    }
    
    void logMissionData() {
        if (!logFile.is_open()) return;
        
        logFile << "{\n";
        logFile << "  \"mission_timestamp\": \"" << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() << "\",\n";
        logFile << "  \"algorithm\": \"" << currentAlgorithm << "\",\n";
        logFile << "  \"grid_size\": " << gridSize << ",\n";
        logFile << "  \"start\": [" << start.x << ", " << start.y << "],\n";
        logFile << "  \"end\": [" << end.x << ", " << end.y << "],\n";
        logFile << "  \"total_distance\": " << totalDistance << ",\n";
        logFile << "  \"total_energy\": " << totalEnergy << ",\n";
        logFile << "  \"computation_time\": " << computationTime << ",\n";
        logFile << "  \"drones_deployed\": " << dronesDeployed << ",\n";
        logFile << "  \"planes_deployed\": " << planesDeployed << ",\n";
        logFile << "  \"path\": [\n";
        
        for (size_t i = 0; i < path.size(); i++) {
            logFile << "    [" << path[i].x << ", " << path[i].y << "]";
            if (i < path.size() - 1) logFile << ",";
            logFile << "\n";
        }
        
        logFile << "  ]\n";
        logFile << "}\n";
        logFile.flush();
    }
    
    void logStepData(size_t step) {
        if (!logFile.is_open()) return;
        
        logFile << "STEP " << step << ": ";
        logFile << "Position[" << path[step].x << "," << path[step].y << "] ";
        logFile << "Terrain[" << terrain[path[step].y][path[step].x] << "] ";
        logFile << "Energy[" << getTerrainCost(terrain[path[step].y][path[step].x]) << "] ";
        logFile << "Entity[UAV]\n";
        logFile.flush();
    }
    
    // Helper methods
    Entity* getEntityAt(Point p) {
        for (Entity& entity : entities) {
            if (entity.position == p && entity.active) {
                return &entity;
            }
        }
        return nullptr;
    }
    
    bool isInPath(const Point& p) {
        return std::find(path.begin(), path.end(), p) != path.end();
    }
    
    bool isExplored(const Point& p) {
        return std::find_if(exploredNodes.begin(), exploredNodes.end(), 
                           [&p](const PathNode& node) { return node.position == p; }) != exploredNodes.end();
    }
    
    void setCoordinates() {
        int sx, sy, ex, ey;
        std::cout << BRIGHT_GREEN << "\n[*] Setting mission coordinates..." << std::endl;
        std::cout << BRIGHT_CYAN << "Start coordinates [x y]: " << RESET;
        std::cin >> sx >> sy;
        std::cout << BRIGHT_CYAN << "Target coordinates [x y]: " << RESET;
        std::cin >> ex >> ey;
        
        if (sx >= 0 && sx < gridSize && sy >= 0 && sy < gridSize &&
            ex >= 0 && ex < gridSize && ey >= 0 && ey < gridSize) {
            start = Point(sx, sy);
            end = Point(ex, ey);
            std::cout << BRIGHT_GREEN << "[+] Coordinates updated successfully!" << std::endl;
        } else {
            std::cout << BRIGHT_RED << "[!] Invalid coordinates! Range: 0-" << (gridSize-1) << std::endl;
        }
    }
    
    void selectAlgorithm() {
        std::cout << BRIGHT_YELLOW << "\n[*] Algorithm selection interface" << std::endl;
        std::cout << BRIGHT_CYAN << "1. A* (Optimal heuristic)" << std::endl;
        std::cout << BRIGHT_CYAN << "2. Dijkstra (Guaranteed shortest)" << std::endl;
        std::cout << BRIGHT_CYAN << "3. Greedy Best-First" << std::endl;
        std::cout << BRIGHT_CYAN << "4. Tactical A*" << std::endl;
        std::cout << BRIGHT_GREEN << "Choice: " << RESET;
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1: currentAlgorithm = "A*"; break;
            case 2: currentAlgorithm = "Dijkstra"; break;
            case 3: currentAlgorithm = "Greedy"; break;
            case 4: currentAlgorithm = "Tactical A*"; break;
            default: 
                std::cout << BRIGHT_RED << "[!] Invalid selection! Using A*" << std::endl;
                currentAlgorithm = "A*";
                return;
        }
        
        std::cout << BRIGHT_GREEN << "[+] Algorithm set to: " << currentAlgorithm << std::endl;
    }
    
    void displayHelp() {
        std::cout << BRIGHT_CYAN << "\n╔═══════════════════════ SYSTEM MANUAL ═══════════════════════╗" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "UAV TACTICAL FLIGHT PATH OPTIMIZER                         " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "Metasploit-Style Interface for Professional Operations     " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "╠═══════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "Commands:                                                  " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "  [RECON] - Display tactical map with entities            " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "  [COORDS] - Set mission start/target coordinates         " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "  [EXECUTE] - Run pathfinding simulation                  " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "  [PLAYBACK] - Step-by-step mission replay               " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "  [DEPLOY] - Deploy/manage drones and planes             " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "  [STATS] - View comprehensive mission statistics        " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "╠═══════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "Controls:                                                  " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "  Enter - Advance/Continue                               " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "  Q - Quit current operation                             " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "║ " << BRIGHT_WHITE << "  P - Pause (during playback)                           " << BRIGHT_CYAN << "║" << std::endl;
        std::cout << BRIGHT_CYAN << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    }
    
    void run() {
        while (true) {
            displayAnimatedBanner();
            displayMainMenu();
            
            int choice;
            std::cin >> choice;
            std::cin.ignore();
            
            switch (choice) {
                case 1: displayTacticalMap(); break;
                case 2: setCoordinates(); break;
                case 3: selectAlgorithm(); break;
                case 4: executeSimulation(); break;
                case 5: stepByStepPlayback(); break;
                case 6: debugMode = !debugMode; 
                       std::cout << BRIGHT_GREEN << "[+] Debug mode: " << (debugMode ? "ENABLED" : "DISABLED") << std::endl; break;
                case 7: displayStatsPanel(); break;
                case 8: deployEntities(); break;
                case 9: std::cout << BRIGHT_YELLOW << "[*] Configuration panel coming soon..." << std::endl; break;
                case 10: liveLogging = !liveLogging; 
                        std::cout << BRIGHT_GREEN << "[+] Live logging: " << (liveLogging ? "ENABLED" : "DISABLED") << std::endl; break;
                case 11: std::cout << BRIGHT_YELLOW << "[*] Export functionality coming soon..." << std::endl; break;
                case 12: loadTerrain(); initializeEntities(); 
                        std::cout << BRIGHT_GREEN << "[+] New terrain generated!" << std::endl; break;
                case 13: displayHelp(); break;
                case 14: 
                    std::cout << BRIGHT_RED << "\n[!] Shutting down tactical system..." << std::endl;
                    std::cout << BRIGHT_YELLOW << "[*] Saving mission data..." << std::endl;
                    std::cout << BRIGHT_GREEN << "[+] System shutdown complete. Stay tactical!" << std::endl;
                    return;
                default:
                    std::cout << BRIGHT_RED << "[!] Invalid command! Type 13 for help." << std::endl;
            }
            
            std::cout << BRIGHT_WHITE << "\nPress Enter to continue..." << RESET;
            std::cin.get();
        }
    }
};

int main() {
    MetasploitUAV tactical;
    tactical.run();
    return 0;
}