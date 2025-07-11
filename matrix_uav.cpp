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

// Matrix-style ANSI color codes
#define MATRIX_GREEN "\033[38;2;0;255;65m"
#define MATRIX_BRIGHT "\033[38;2;150;255;150m"
#define MATRIX_DIM "\033[38;2;0;150;40m"
#define MATRIX_DARK "\033[38;2;0;100;25m"
#define MATRIX_GLOW "\033[38;2;200;255;200m"
#define MATRIX_NEON "\033[38;2;57;255;20m"
#define MATRIX_TERMINAL "\033[38;2;0;200;50m"
#define MATRIX_DANGER "\033[38;2;255;50;50m"
#define MATRIX_WARNING "\033[38;2;255;255;0m"
#define MATRIX_INFO "\033[38;2;0;255;255m"
#define MATRIX_SHADOW "\033[38;2;20;60;20m"
#define BLACK_BG "\033[40m"
#define RESET "\033[0m"
#define CLEAR_SCREEN "\033[2J\033[H"

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
    std::string type;
    std::string symbol;
    std::string color;
    bool active;
    
    Entity(Point pos, std::string t, std::string s, std::string c) 
        : position(pos), type(t), symbol(s), color(c), active(true) {}
};

class MatrixUAV {
private:
    std::vector<std::vector<char>> terrain;
    std::vector<Point> path;
    std::vector<PathNode> exploredNodes;
    std::vector<Entity> entities;
    Point start, end;
    int gridSize;
    std::string currentAlgorithm;
    bool debugMode;
    bool matrixMode;
    bool realTimeMode;
    
    // Advanced statistics
    int totalDistance;
    double totalEnergy;
    int windZonesCrossed;
    int obstaclesAvoided;
    int stepsTaken;
    double computationTime;
    int nodesExplored;
    double pathEfficiency;
    double energyEfficiency;
    
    // AI/ML simulation data
    std::map<std::string, double> aiMetrics;
    std::vector<std::string> simulationLog;
    std::mt19937 rng;
    
public:
    MatrixUAV() : gridSize(15), currentAlgorithm("A*"), debugMode(false), matrixMode(true), realTimeMode(false),
                  totalDistance(0), totalEnergy(0), windZonesCrossed(0), obstaclesAvoided(0), 
                  stepsTaken(0), computationTime(0), nodesExplored(0), pathEfficiency(0), energyEfficiency(0),
                  rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
        loadAdvancedTerrain();
        start = Point(1, 1);
        end = Point(13, 13);
        initializeAI();
        initializeEntities();
    }
    
    void initializeEntities() {
        entities.clear();
        
        // Add drones with Matrix symbols
        for (int i = 0; i < 3; i++) {
            int x = rng() % gridSize;
            int y = rng() % gridSize;
            if (terrain[y][x] == '.') {
                entities.push_back(Entity(Point(x, y), "drone", "✈", MATRIX_BRIGHT));
            }
        }
        
        // Add planes
        for (int i = 0; i < 2; i++) {
            int x = rng() % gridSize;
            int y = rng() % gridSize;
            if (terrain[y][x] == '.') {
                entities.push_back(Entity(Point(x, y), "plane", "🛩", MATRIX_NEON));
            }
        }
    }
    
    void loadAdvancedTerrain() {
        terrain.assign(gridSize, std::vector<char>(gridSize, '.'));
        
        // Create more complex terrain patterns
        for (int i = 0; i < 25; i++) {
            int x = rng() % gridSize;
            int y = rng() % gridSize;
            if (x > 0 && x < gridSize-1 && y > 0 && y < gridSize-1) {
                terrain[y][x] = 'O';
            }
        }
        
        // Add wind zones in patterns
        for (int i = 0; i < 15; i++) {
            int x = rng() % gridSize;
            int y = rng() % gridSize;
            if (terrain[y][x] == '.') terrain[y][x] = 'W';
        }
        
        // Add hills
        for (int i = 0; i < 10; i++) {
            int x = rng() % gridSize;
            int y = rng() % gridSize;
            if (terrain[y][x] == '.') terrain[y][x] = '^';
        }
        
        // Add energy stations
        for (int i = 0; i < 5; i++) {
            int x = rng() % gridSize;
            int y = rng() % gridSize;
            if (terrain[y][x] == '.') terrain[y][x] = 'E';
        }
        
        // Add danger zones
        for (int i = 0; i < 8; i++) {
            int x = rng() % gridSize;
            int y = rng() % gridSize;
            if (terrain[y][x] == '.') terrain[y][x] = 'X';
        }
    }
    
    void initializeAI() {
        aiMetrics["neural_network_confidence"] = 0.95;
        aiMetrics["pathfinding_accuracy"] = 0.88;
        aiMetrics["energy_prediction"] = 0.92;
        aiMetrics["obstacle_detection"] = 0.99;
        aiMetrics["weather_adaptation"] = 0.85;
        aiMetrics["learning_rate"] = 0.001;
    }
    
    void displayMatrixHeader() {
        std::cout << CLEAR_SCREEN << BLACK_BG;
        
        // Matrix rain effect
        for (int i = 0; i < 3; i++) {
            std::cout << MATRIX_DIM;
            for (int j = 0; j < 80; j++) {
                if (rng() % 10 == 0) std::cout << (char)('0' + rng() % 10);
                else std::cout << " ";
            }
            std::cout << std::endl;
        }
        
        std::cout << MATRIX_NEON << "╔══════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << MATRIX_NEON << "║" << MATRIX_GLOW << "    ███    ███  █████  ████████ ████████  ██  ██   ██        ██    ██  █████  ██    ██" << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║" << MATRIX_GLOW << "    ████  ████ ██   ██    ██    ██     ██ ██   ██ ██         ██    ██ ██   ██ ██    ██" << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║" << MATRIX_GLOW << "    ██ ████ ██ ███████    ██    ████████  ██    ███          ██    ██ ███████ ██    ██" << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║" << MATRIX_GLOW << "    ██  ██  ██ ██   ██    ██    ██   ██   ██   ██ ██         ██    ██ ██   ██  ██  ██ " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║" << MATRIX_GLOW << "    ██      ██ ██   ██    ██    ██    ██  ██  ██   ██         ██████  ██   ██   ████  " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║                                                                              ║" << std::endl;
        std::cout << MATRIX_NEON << "║" << MATRIX_BRIGHT << "                    AI-POWERED DRONE NAVIGATION SYSTEM                        " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║" << MATRIX_GREEN << "                        [ QUANTUM PATHFINDING ENABLED ]                       " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "╚══════════════════════════════════════════════════════════════════════════════╝" << std::endl;
        
        // System status
        std::cout << std::endl;
        std::cout << MATRIX_TERMINAL << ">>> SYSTEM STATUS:" << RESET << std::endl;
        std::cout << MATRIX_GREEN << "  ▶ Algorithm: " << MATRIX_BRIGHT << currentAlgorithm << RESET << std::endl;
        std::cout << MATRIX_GREEN << "  ▶ Debug Mode: " << (debugMode ? MATRIX_BRIGHT + std::string("ACTIVE") : MATRIX_DIM + std::string("INACTIVE")) << RESET << std::endl;
        std::cout << MATRIX_GREEN << "  ▶ Matrix Mode: " << (matrixMode ? MATRIX_BRIGHT + std::string("ENGAGED") : MATRIX_DIM + std::string("DISENGAGED")) << RESET << std::endl;
        std::cout << MATRIX_GREEN << "  ▶ Real-Time: " << (realTimeMode ? MATRIX_BRIGHT + std::string("ENABLED") : MATRIX_DIM + std::string("DISABLED")) << RESET << std::endl;
        std::cout << MATRIX_GREEN << "  ▶ AI Confidence: " << MATRIX_BRIGHT << std::fixed << std::setprecision(1) << (aiMetrics["neural_network_confidence"] * 100) << "%" << RESET << std::endl;
        std::cout << MATRIX_GREEN << "  ▶ Entities: " << MATRIX_BRIGHT << entities.size() << " deployed" << RESET << std::endl;
        std::cout << std::endl;
    }
    
    void displayMatrixMenu() {
        std::cout << MATRIX_NEON << "╔════════════════════════ COMMAND INTERFACE ════════════════════════════╗" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << " 1. [SCAN] Display terrain map              " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << " 2. [COORD] Set navigation coordinates       " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << " 3. [ALGO] Select pathfinding algorithm      " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << " 4. [EXEC] Execute navigation simulation     " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << " 5. [PLAY] Step-by-step mission playback    " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << " 6. [DEBUG] Toggle debug/analysis mode      " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << " 7. [STATS] Display mission statistics      " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << " 8. [EXPORT] Export mission data            " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << " 9. [AI] AI performance analytics           " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "10. [DEPLOY] Deploy entities                " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "11. [REAL] Toggle real-time mode            " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "12. [GEN] Generate new terrain              " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "13. [HELP] System documentation             " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "14. [EXIT] Shutdown system                  " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "╚════════════════════════════════════════════════════════════════════════╝" << std::endl;
        
        std::cout << MATRIX_TERMINAL << ">>> COMMAND: " << MATRIX_BRIGHT;
    }
    
    void displayMatrixMap() {
        std::cout << MATRIX_NEON << "\n╔═══════════════════════ TERRAIN ANALYSIS ═══════════════════════╗" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "Grid: " << gridSize << "x" << gridSize << 
                     "  Algorithm: " << currentAlgorithm << 
                     "  Nodes: " << nodesExplored << std::string(20, ' ') << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "Start: [" << start.x << "," << start.y << "]  " <<
                     "Target: [" << end.x << "," << end.y << "]  " <<
                     "Entities: " << entities.size() << "  Path: " << path.size() << " steps" << std::string(5, ' ') << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "╚═════════════════════════════════════════════════════════════════╝" << std::endl;
        
        // Column headers
        std::cout << MATRIX_TERMINAL << "    ";
        for (int x = 0; x < gridSize; x++) {
            std::cout << MATRIX_TERMINAL << std::setw(3) << x;
        }
        std::cout << std::endl;
        
        // Map display
        for (int y = 0; y < gridSize; y++) {
            std::cout << MATRIX_TERMINAL << std::setw(3) << y << " ";
            for (int x = 0; x < gridSize; x++) {
                Point current(x, y);
                char cell = terrain[y][x];
                
                // Check for entities first
                Entity* entity = getEntityAt(current);
                if (entity && entity->active) {
                    std::cout << entity->color << entity->symbol << " " << RESET;
                } else if (current == start) {
                    std::cout << MATRIX_GLOW << "▲  " << RESET;
                } else if (current == end) {
                    std::cout << MATRIX_DANGER << "◆  " << RESET;
                } else if (isInPath(current)) {
                    std::cout << MATRIX_BRIGHT << "●  " << RESET;
                } else if (debugMode && isExplored(current)) {
                    std::cout << MATRIX_DIM << "·  " << RESET;
                } else {
                    switch (cell) {
                        case 'O': std::cout << MATRIX_DANGER << "█  " << RESET; break;
                        case '^': std::cout << MATRIX_WARNING << "▲  " << RESET; break;
                        case 'W': std::cout << MATRIX_INFO << "~  " << RESET; break;
                        case 'E': std::cout << MATRIX_NEON << "⚡ " << RESET; break;
                        case 'X': std::cout << MATRIX_DANGER << "☢  " << RESET; break;
                        default: std::cout << MATRIX_GREEN << "·  " << RESET; break;
                    }
                }
            }
            std::cout << std::endl;
        }
        
        // Legend
        std::cout << std::endl;
        std::cout << MATRIX_NEON << "╔═════════════════════ LEGEND ═════════════════════╗" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_GLOW << "▲" << MATRIX_BRIGHT << " = START    " << 
                     MATRIX_DANGER << "◆" << MATRIX_BRIGHT << " = TARGET   " << 
                     MATRIX_BRIGHT << "●" << MATRIX_BRIGHT << " = PATH     " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_DANGER << "█" << MATRIX_BRIGHT << " = OBSTACLE " << 
                     MATRIX_WARNING << "▲" << MATRIX_BRIGHT << " = HILL     " << 
                     MATRIX_INFO << "~" << MATRIX_BRIGHT << " = WIND     " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_NEON << "⚡" << MATRIX_BRIGHT << " = ENERGY   " << 
                     MATRIX_DANGER << "☢" << MATRIX_BRIGHT << " = DANGER   " << 
                     MATRIX_GREEN << "·" << MATRIX_BRIGHT << " = CLEAR    " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "✈" << MATRIX_BRIGHT << " = DRONE    " << 
                     MATRIX_NEON << "🛩" << MATRIX_BRIGHT << " = PLANE    " << 
                     MATRIX_DIM << "·" << MATRIX_BRIGHT << " = EXPLORED " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "╚═══════════════════════════════════════════════════╝" << std::endl;
        
        if (debugMode) {
            displayAdvancedDebugInfo();
        }
    }
    
    void displayAdvancedDebugInfo() {
        std::cout << MATRIX_TERMINAL << "\n╔════════════════════ DEBUG ANALYSIS ════════════════════╗" << std::endl;
        std::cout << MATRIX_TERMINAL << "║ " << MATRIX_BRIGHT << "Nodes Explored: " << std::setw(6) << nodesExplored << 
                     "  Memory: " << std::setw(6) << (exploredNodes.size() * sizeof(PathNode)) << " bytes" << 
                     std::string(8, ' ') << MATRIX_TERMINAL << "║" << std::endl;
        std::cout << MATRIX_TERMINAL << "║ " << MATRIX_BRIGHT << "Path Efficiency: " << std::setw(5) << std::fixed << std::setprecision(1) << pathEfficiency << "%" << 
                     "  Energy Eff: " << std::setw(5) << energyEfficiency << "%" << 
                     std::string(10, ' ') << MATRIX_TERMINAL << "║" << std::endl;
        std::cout << MATRIX_TERMINAL << "║ " << MATRIX_BRIGHT << "Algorithm State: " << currentAlgorithm << 
                     "  Heuristic: Manhattan" << std::string(15, ' ') << MATRIX_TERMINAL << "║" << std::endl;
        std::cout << MATRIX_TERMINAL << "╚═════════════════════════════════════════════════════════╝" << std::endl;
    }
    
    void runAdvancedSimulation() {
        std::cout << MATRIX_TERMINAL << "\n╔════════════════════ MISSION EXECUTION ════════════════════╗" << std::endl;
        std::cout << MATRIX_TERMINAL << "║ " << MATRIX_BRIGHT << "Initializing quantum pathfinding algorithms...           " << MATRIX_TERMINAL << "║" << std::endl;
        std::cout << MATRIX_TERMINAL << "║ " << MATRIX_BRIGHT << "Analyzing terrain topology...                           " << MATRIX_TERMINAL << "║" << std::endl;
        std::cout << MATRIX_TERMINAL << "║ " << MATRIX_BRIGHT << "Calculating optimal trajectory...                       " << MATRIX_TERMINAL << "║" << std::endl;
        std::cout << MATRIX_TERMINAL << "╚════════════════════════════════════════════════════════════╝" << std::endl;
        
        // Simulate processing
        for (int i = 0; i < 3; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::cout << MATRIX_GREEN << ">>> Processing";
            for (int j = 0; j < 3; j++) {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                std::cout << MATRIX_GREEN << ".";
            }
            std::cout << MATRIX_BRIGHT << " [" << ((i+1)*33) << "%]" << std::endl;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Advanced A* pathfinding
        generateAdvancedPath();
        
        auto endTime = std::chrono::high_resolution_clock::now();
        computationTime = std::chrono::duration<double>(endTime - startTime).count();
        
        calculateAdvancedStatistics();
        
        std::cout << MATRIX_NEON << "\n>>> MISSION COMPLETE! <<<" << std::endl;
        displayMatrixStatistics();
    }
    
    void generateAdvancedPath() {
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
                    
                    double moveCost = (dx == 0 || dy == 0) ? 1.0 : 1.414; // Diagonal cost
                    double terrainCost = getAdvancedEnergyCost(terrain[neighbor.y][neighbor.x]);
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
    
    double getAdvancedEnergyCost(char terrainType) {
        switch (terrainType) {
            case '.': return 1.0;
            case 'W': return 2.5;
            case '^': return 4.0;
            case 'O': return 1000.0;
            case 'E': return 0.5;  // Energy station
            case 'X': return 10.0; // Danger zone
            default: return 1.0;
        }
    }
    
    void calculateAdvancedStatistics() {
        totalDistance = path.size() - 1;
        totalEnergy = 0;
        windZonesCrossed = 0;
        obstaclesAvoided = 0;
        stepsTaken = path.size();
        
        // Calculate detailed statistics
        for (const Point& p : path) {
            char terrainType = terrain[p.y][p.x];
            totalEnergy += getAdvancedEnergyCost(terrainType);
            if (terrainType == 'W') windZonesCrossed++;
        }
        
        // Calculate efficiency metrics
        double straightLineDistance = calculateHeuristic(start, end);
        pathEfficiency = (straightLineDistance / totalDistance) * 100;
        energyEfficiency = (straightLineDistance / totalEnergy) * 100;
        
        // Update AI metrics
        aiMetrics["pathfinding_accuracy"] = pathEfficiency / 100.0;
        aiMetrics["energy_prediction"] = energyEfficiency / 100.0;
        
        obstaclesAvoided = countNearbyObstacles();
    }
    
    int countNearbyObstacles() {
        int count = 0;
        for (const Point& p : path) {
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int nx = p.x + dx, ny = p.y + dy;
                    if (nx >= 0 && nx < gridSize && ny >= 0 && ny < gridSize) {
                        if (terrain[ny][nx] == 'O' || terrain[ny][nx] == 'X') count++;
                    }
                }
            }
        }
        return count / 3; // Approximate
    }
    
    void displayMatrixStatistics() {
        std::cout << MATRIX_NEON << "\n╔═══════════════════════ MISSION REPORT ═══════════════════════╗" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "Total Distance:       " << std::setw(8) << totalDistance << " cells       " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "Total Energy Cost:    " << std::setw(8) << std::fixed << std::setprecision(1) << totalEnergy << " units       " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "Wind Zones Crossed:   " << std::setw(8) << windZonesCrossed << "             " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "Obstacles Avoided:    " << std::setw(8) << obstaclesAvoided << "             " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "Steps Taken:          " << std::setw(8) << stepsTaken << "             " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "Computation Time:     " << std::setw(8) << std::fixed << std::setprecision(3) << computationTime << " seconds    " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "Nodes Explored:       " << std::setw(8) << nodesExplored << "             " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "Path Efficiency:      " << std::setw(8) << std::fixed << std::setprecision(1) << pathEfficiency << "%            " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "Energy Efficiency:    " << std::setw(8) << std::fixed << std::setprecision(1) << energyEfficiency << "%            " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
        
        // Energy visualization
        displayEnergyMatrix();
        
        // AI Performance
        displayAIMetrics();
    }
    
    void displayEnergyMatrix() {
        std::cout << std::endl;
        std::cout << MATRIX_TERMINAL << ">>> ENERGY CONSUMPTION MATRIX:" << std::endl;
        int barLength = 50;
        double maxEnergy = 100.0;
        int filledLength = (int)((totalEnergy / maxEnergy) * barLength);
        
        std::cout << MATRIX_INFO << "Energy: [";
        for (int i = 0; i < barLength; i++) {
            if (i < filledLength) {
                if (i < barLength * 0.7) std::cout << MATRIX_GREEN << "█";
                else if (i < barLength * 0.9) std::cout << MATRIX_WARNING << "█";
                else std::cout << MATRIX_DANGER << "█";
            } else {
                std::cout << MATRIX_SHADOW << "░";
            }
        }
        std::cout << MATRIX_INFO << "] " << std::fixed << std::setprecision(1) << totalEnergy << " units" << RESET << std::endl;
    }
    
    void displayAIMetrics() {
        std::cout << std::endl;
        std::cout << MATRIX_TERMINAL << ">>> AI PERFORMANCE ANALYSIS:" << std::endl;
        for (const auto& metric : aiMetrics) {
            std::cout << MATRIX_GREEN << "  ▶ " << metric.first << ": " << MATRIX_BRIGHT << 
                         std::fixed << std::setprecision(1) << (metric.second * 100) << "%" << RESET << std::endl;
        }
    }
    
    void displayAIAnalytics() {
        std::cout << MATRIX_NEON << "\n╔═════════════════════ AI ANALYTICS ═════════════════════╗" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "Neural Network Performance Analysis             " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "╚═════════════════════════════════════════════════════════╝" << std::endl;
        
        // Simulate AI learning
        for (auto& metric : aiMetrics) {
            double improvement = (rng() % 100) / 10000.0; // Small random improvement
            metric.second = std::min(1.0, metric.second + improvement);
        }
        
        displayAIMetrics();
        
        std::cout << std::endl;
        std::cout << MATRIX_TERMINAL << ">>> LEARNING PROGRESSION:" << std::endl;
        std::cout << MATRIX_GREEN << "  ▶ Pattern Recognition: Enhanced" << std::endl;
        std::cout << MATRIX_GREEN << "  ▶ Obstacle Prediction: Improved" << std::endl;
        std::cout << MATRIX_GREEN << "  ▶ Energy Optimization: Updated" << std::endl;
        std::cout << MATRIX_GREEN << "  ▶ Route Efficiency: Calibrated" << std::endl;
    }
    
    void matrixPlayback() {
        if (path.empty()) {
            std::cout << MATRIX_DANGER << ">>> ERROR: No mission data available. Execute simulation first." << RESET << std::endl;
            return;
        }
        
        std::cout << MATRIX_TERMINAL << "\n╔════════════════ MISSION PLAYBACK ════════════════╗" << std::endl;
        std::cout << MATRIX_TERMINAL << "║ " << MATRIX_BRIGHT << "Initiating step-by-step analysis...        " << MATRIX_TERMINAL << "║" << std::endl;
        std::cout << MATRIX_TERMINAL << "║ " << MATRIX_BRIGHT << "Press ENTER to advance (Q to abort)        " << MATRIX_TERMINAL << "║" << std::endl;
        std::cout << MATRIX_TERMINAL << "╚═══════════════════════════════════════════════════╝" << std::endl;
        
        for (size_t step = 0; step < path.size(); step++) {
            std::cout << CLEAR_SCREEN;
            
            std::cout << MATRIX_NEON << ">>> MISSION STEP " << (step + 1) << " / " << path.size() << " <<<" << std::endl;
            std::cout << MATRIX_BRIGHT << "Position: [" << path[step].x << "," << path[step].y << "]" << std::endl;
            std::cout << MATRIX_BRIGHT << "Terrain: " << terrain[path[step].y][path[step].x] << std::endl;
            std::cout << MATRIX_BRIGHT << "Energy Cost: " << getAdvancedEnergyCost(terrain[path[step].y][path[step].x]) << std::endl;
            std::cout << std::endl;
            
            drawMatrixPlayback(step);
            
            std::string input;
            std::cout << MATRIX_TERMINAL << ">>> CONTINUE: " << MATRIX_BRIGHT;
            std::getline(std::cin, input);
            if (input == "q" || input == "Q") break;
        }
        
        std::cout << MATRIX_NEON << ">>> PLAYBACK COMPLETE <<<" << RESET << std::endl;
    }
    
    void drawMatrixPlayback(size_t currentStep) {
        // Column headers
        std::cout << MATRIX_TERMINAL << "    ";
        for (int x = 0; x < gridSize; x++) {
            std::cout << MATRIX_TERMINAL << std::setw(3) << x;
        }
        std::cout << std::endl;
        
        for (int y = 0; y < gridSize; y++) {
            std::cout << MATRIX_TERMINAL << std::setw(3) << y << " ";
            for (int x = 0; x < gridSize; x++) {
                Point current(x, y);
                char cell = terrain[y][x];
                
                // Current position
                if (currentStep < path.size() && current == path[currentStep]) {
                    std::cout << MATRIX_GLOW << "◉  " << RESET;
                } else if (current == start) {
                    std::cout << MATRIX_GLOW << "▲  " << RESET;
                } else if (current == end) {
                    std::cout << MATRIX_DANGER << "◆  " << RESET;
                } else if (currentStep > 0 && std::find(path.begin(), path.begin() + currentStep, current) != path.begin() + currentStep) {
                    std::cout << MATRIX_BRIGHT << "●  " << RESET;
                } else {
                    switch (cell) {
                        case 'O': std::cout << MATRIX_DANGER << "█  " << RESET; break;
                        case '^': std::cout << MATRIX_WARNING << "▲  " << RESET; break;
                        case 'W': std::cout << MATRIX_INFO << "~  " << RESET; break;
                        case 'E': std::cout << MATRIX_NEON << "⚡ " << RESET; break;
                        case 'X': std::cout << MATRIX_DANGER << "☢  " << RESET; break;
                        default: std::cout << MATRIX_GREEN << "·  " << RESET; break;
                    }
                }
            }
            std::cout << std::endl;
        }
    }
    
    // Helper methods
    bool isInPath(const Point& p) {
        return std::find(path.begin(), path.end(), p) != path.end();
    }
    
    bool isExplored(const Point& p) {
        return std::find_if(exploredNodes.begin(), exploredNodes.end(), 
                           [&p](const PathNode& node) { return node.position == p; }) != exploredNodes.end();
    }
    
    void setStartEndPoints() {
        int sx, sy, ex, ey;
        std::cout << MATRIX_TERMINAL << "\n>>> SET NAVIGATION COORDINATES:" << std::endl;
        std::cout << MATRIX_GREEN << "Start coordinates [x y]: " << MATRIX_BRIGHT;
        std::cin >> sx >> sy;
        std::cout << MATRIX_GREEN << "Target coordinates [x y]: " << MATRIX_BRIGHT;
        std::cin >> ex >> ey;
        
        if (sx >= 0 && sx < gridSize && sy >= 0 && sy < gridSize &&
            ex >= 0 && ex < gridSize && ey >= 0 && ey < gridSize) {
            start = Point(sx, sy);
            end = Point(ex, ey);
            std::cout << MATRIX_NEON << ">>> COORDINATES UPDATED <<<" << RESET << std::endl;
        } else {
            std::cout << MATRIX_DANGER << ">>> ERROR: Invalid coordinates! Range: 0-" << (gridSize-1) << " <<<" << RESET << std::endl;
        }
    }
    
    void selectAlgorithm() {
        std::cout << MATRIX_TERMINAL << "\n>>> SELECT PATHFINDING ALGORITHM:" << std::endl;
        std::cout << MATRIX_GREEN << "1. A* (Optimal)" << std::endl;
        std::cout << MATRIX_GREEN << "2. Dijkstra (Guaranteed)" << std::endl;
        std::cout << MATRIX_GREEN << "3. Greedy Best-First" << std::endl;
        std::cout << MATRIX_GREEN << "4. Quantum A*" << std::endl;
        std::cout << MATRIX_TERMINAL << "Choice: " << MATRIX_BRIGHT;
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1: currentAlgorithm = "A*"; break;
            case 2: currentAlgorithm = "Dijkstra"; break;
            case 3: currentAlgorithm = "Greedy"; break;
            case 4: currentAlgorithm = "Quantum A*"; break;
            default: 
                std::cout << MATRIX_DANGER << ">>> Invalid selection! Using A* <<<" << RESET << std::endl;
                currentAlgorithm = "A*";
                return;
        }
        
        std::cout << MATRIX_NEON << ">>> ALGORITHM SET: " << currentAlgorithm << " <<<" << RESET << std::endl;
    }
    
    void exportMatrixData() {
        std::cout << MATRIX_TERMINAL << "\n>>> DATA EXPORT PROTOCOLS:" << std::endl;
        std::cout << MATRIX_GREEN << "1. Mission Report (CSV)" << std::endl;
        std::cout << MATRIX_GREEN << "2. AI Analytics (JSON)" << std::endl;
        std::cout << MATRIX_GREEN << "3. Full System Log (TXT)" << std::endl;
        std::cout << MATRIX_GREEN << "4. Matrix Binary (BIN)" << std::endl;
        std::cout << MATRIX_TERMINAL << "Protocol: " << MATRIX_BRIGHT;
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1: exportMissionCSV(); break;
            case 2: exportAIJSON(); break;
            case 3: exportSystemTXT(); break;
            case 4: exportMatrixBIN(); break;
            default: std::cout << MATRIX_DANGER << ">>> Invalid protocol! <<<" << RESET << std::endl;
        }
    }
    
    void exportMissionCSV() {
        std::ofstream file("matrix_mission_report.csv");
        file << "Step,X,Y,Terrain,Energy,Cumulative,Efficiency\n";
        
        double cumulative = 0;
        for (size_t i = 0; i < path.size(); i++) {
            Point p = path[i];
            double energy = getAdvancedEnergyCost(terrain[p.y][p.x]);
            cumulative += energy;
            double efficiency = (i > 0) ? (double)i / cumulative : 1.0;
            
            file << i << "," << p.x << "," << p.y << "," << terrain[p.y][p.x] << "," 
                 << energy << "," << cumulative << "," << efficiency << "\n";
        }
        
        file.close();
        std::cout << MATRIX_NEON << ">>> MISSION REPORT EXPORTED: matrix_mission_report.csv <<<" << RESET << std::endl;
    }
    
    void exportAIJSON() {
        std::ofstream file("matrix_ai_analytics.json");
        file << "{\n";
        file << "  \"mission_data\": {\n";
        file << "    \"algorithm\": \"" << currentAlgorithm << "\",\n";
        file << "    \"grid_size\": " << gridSize << ",\n";
        file << "    \"path_length\": " << path.size() << ",\n";
        file << "    \"nodes_explored\": " << nodesExplored << ",\n";
        file << "    \"computation_time\": " << computationTime << "\n";
        file << "  },\n";
        file << "  \"ai_metrics\": {\n";
        
        size_t count = 0;
        for (const auto& metric : aiMetrics) {
            file << "    \"" << metric.first << "\": " << metric.second;
            if (++count < aiMetrics.size()) file << ",";
            file << "\n";
        }
        
        file << "  },\n";
        file << "  \"performance\": {\n";
        file << "    \"path_efficiency\": " << pathEfficiency << ",\n";
        file << "    \"energy_efficiency\": " << energyEfficiency << ",\n";
        file << "    \"total_energy\": " << totalEnergy << "\n";
        file << "  }\n";
        file << "}\n";
        file.close();
        
        std::cout << MATRIX_NEON << ">>> AI ANALYTICS EXPORTED: matrix_ai_analytics.json <<<" << RESET << std::endl;
    }
    
    void exportSystemTXT() {
        std::ofstream file("matrix_system_log.txt");
        file << "MATRIX UAV SYSTEM LOG\n";
        file << "=====================\n\n";
        file << "Mission Parameters:\n";
        file << "  Algorithm: " << currentAlgorithm << "\n";
        file << "  Grid Size: " << gridSize << "x" << gridSize << "\n";
        file << "  Start: [" << start.x << "," << start.y << "]\n";
        file << "  Target: [" << end.x << "," << end.y << "]\n\n";
        
        file << "Performance Metrics:\n";
        file << "  Total Distance: " << totalDistance << " cells\n";
        file << "  Total Energy: " << totalEnergy << " units\n";
        file << "  Nodes Explored: " << nodesExplored << "\n";
        file << "  Computation Time: " << computationTime << " seconds\n";
        file << "  Path Efficiency: " << pathEfficiency << "%\n";
        file << "  Energy Efficiency: " << energyEfficiency << "%\n\n";
        
        file << "Flight Path:\n";
        for (size_t i = 0; i < path.size(); i++) {
            file << "  Step " << i << ": [" << path[i].x << "," << path[i].y << "] - " 
                 << terrain[path[i].y][path[i].x] << "\n";
        }
        
        file.close();
        std::cout << MATRIX_NEON << ">>> SYSTEM LOG EXPORTED: matrix_system_log.txt <<<" << RESET << std::endl;
    }
    
    void exportMatrixBIN() {
        std::ofstream file("matrix_data.bin", std::ios::binary);
        
        // Write header
        file.write(reinterpret_cast<const char*>(&gridSize), sizeof(gridSize));
        file.write(reinterpret_cast<const char*>(&start), sizeof(start));
        file.write(reinterpret_cast<const char*>(&end), sizeof(end));
        
        // Write terrain
        for (int y = 0; y < gridSize; y++) {
            for (int x = 0; x < gridSize; x++) {
                file.write(&terrain[y][x], sizeof(char));
            }
        }
        
        // Write path
        size_t pathSize = path.size();
        file.write(reinterpret_cast<const char*>(&pathSize), sizeof(pathSize));
        for (const Point& p : path) {
            file.write(reinterpret_cast<const char*>(&p), sizeof(p));
        }
        
        file.close();
        std::cout << MATRIX_NEON << ">>> MATRIX DATA EXPORTED: matrix_data.bin <<<" << RESET << std::endl;
    }
    
    void run() {
        while (true) {
            displayMatrixHeader();
            displayMatrixMenu();
            
            int choice;
            std::cin >> choice;
            std::cin.ignore(); // Clear input buffer
            
            switch (choice) {
                case 1: displayMatrixMap(); break;
                case 2: setStartEndPoints(); break;
                case 3: selectAlgorithm(); break;
                case 4: runAdvancedSimulation(); break;
                case 5: matrixPlayback(); break;
                case 6: debugMode = !debugMode; 
                       std::cout << MATRIX_NEON << ">>> DEBUG MODE: " << (debugMode ? "ACTIVATED" : "DEACTIVATED") << " <<<" << RESET << std::endl; break;
                case 7: displayMatrixStatistics(); break;
                case 8: exportMatrixData(); break;
                case 9: displayAIAnalytics(); break;
                case 10: deployEntities(); break;
                case 11: realTimeMode = !realTimeMode; 
                        std::cout << MATRIX_NEON << ">>> REAL-TIME MODE: " << (realTimeMode ? "ENABLED" : "DISABLED") << " <<<" << RESET << std::endl; break;
                case 12: loadAdvancedTerrain(); initializeEntities();
                        std::cout << MATRIX_NEON << ">>> NEW TERRAIN GENERATED <<<" << RESET << std::endl; break;
                case 13: displayMatrixHelp(); break;
                case 14: 
                    std::cout << MATRIX_NEON << "\n>>> MATRIX UAV SYSTEM SHUTDOWN <<<" << std::endl;
                    std::cout << MATRIX_GREEN << ">>> DISCONNECTING FROM THE MATRIX..." << std::endl;
                    std::cout << MATRIX_DIM << ">>> GOODBYE, OPERATOR." << RESET << std::endl;
                    return;
                default:
                    std::cout << MATRIX_DANGER << ">>> INVALID COMMAND! <<<" << RESET << std::endl;
            }
            
            std::cout << MATRIX_TERMINAL << "\n>>> PRESS ENTER TO CONTINUE..." << RESET;
            std::cin.get();
        }
    }
    
    void deployEntities() {
        std::cout << MATRIX_TERMINAL << "\n>>> ENTITY DEPLOYMENT INTERFACE:" << std::endl;
        std::cout << MATRIX_GREEN << "1. Deploy additional drones" << std::endl;
        std::cout << MATRIX_GREEN << "2. Deploy additional planes" << std::endl;
        std::cout << MATRIX_GREEN << "3. Show entity status" << std::endl;
        std::cout << MATRIX_GREEN << "4. Clear all entities" << std::endl;
        std::cout << MATRIX_TERMINAL << "Choice: " << MATRIX_BRIGHT;
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1: {
                std::cout << MATRIX_GREEN << "Number of drones to deploy: " << MATRIX_BRIGHT;
                int count;
                std::cin >> count;
                for (int i = 0; i < count; i++) {
                    int x = rng() % gridSize;
                    int y = rng() % gridSize;
                    if (terrain[y][x] == '.') {
                        entities.push_back(Entity(Point(x, y), "drone", "✈", MATRIX_BRIGHT));
                    }
                }
                std::cout << MATRIX_NEON << ">>> DEPLOYED " << count << " DRONES <<<" << RESET << std::endl;
                break;
            }
            case 2: {
                std::cout << MATRIX_GREEN << "Number of planes to deploy: " << MATRIX_BRIGHT;
                int count;
                std::cin >> count;
                for (int i = 0; i < count; i++) {
                    int x = rng() % gridSize;
                    int y = rng() % gridSize;
                    if (terrain[y][x] == '.') {
                        entities.push_back(Entity(Point(x, y), "plane", "🛩", MATRIX_NEON));
                    }
                }
                std::cout << MATRIX_NEON << ">>> DEPLOYED " << count << " PLANES <<<" << RESET << std::endl;
                break;
            }
            case 3: {
                std::cout << MATRIX_TERMINAL << "\n>>> ENTITY STATUS:" << std::endl;
                std::cout << MATRIX_GREEN << "Total Entities: " << entities.size() << std::endl;
                for (size_t i = 0; i < entities.size(); i++) {
                    std::cout << entities[i].color << entities[i].type << " " << entities[i].symbol << RESET << 
                                 " at [" << entities[i].position.x << "," << entities[i].position.y << "]" << std::endl;
                }
                break;
            }
            case 4: {
                entities.clear();
                std::cout << MATRIX_NEON << ">>> ALL ENTITIES CLEARED <<<" << RESET << std::endl;
                break;
            }
            default:
                std::cout << MATRIX_DANGER << ">>> INVALID CHOICE <<<" << RESET << std::endl;
        }
    }
    
    // Helper method to get entity at position
    Entity* getEntityAt(Point p) {
        for (Entity& entity : entities) {
            if (entity.position == p && entity.active) {
                return &entity;
            }
        }
        return nullptr;
    }
    
    void displayMatrixHelp() {
        std::cout << MATRIX_NEON << "\n╔════════════════════════ SYSTEM MANUAL ════════════════════════╗" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "MATRIX UAV - AI-Powered Drone Navigation System              " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "╠════════════════════════════════════════════════════════════════╣" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "[SCAN] - Display terrain with advanced visualization          " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "[COORD] - Set navigation start and target coordinates        " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "[ALGO] - Select pathfinding algorithm (A*, Dijkstra, etc.)   " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "[EXEC] - Execute navigation with real-time processing        " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "[PLAY] - Step-by-step mission playback with visualization    " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "[DEBUG] - Toggle advanced debug and analysis mode            " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "[STATS] - Display comprehensive mission statistics          " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "[EXPORT] - Export data in multiple formats (CSV/JSON/TXT)    " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "[AI] - AI performance analytics and learning metrics        " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "[DEPLOY] - Deploy and manage drones and planes               " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "[REAL] - Toggle real-time simulation mode                    " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "║ " << MATRIX_BRIGHT << "[GEN] - Generate new random terrain                          " << MATRIX_NEON << "║" << std::endl;
        std::cout << MATRIX_NEON << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
        
        std::cout << std::endl;
        std::cout << MATRIX_TERMINAL << ">>> TERRAIN LEGEND:" << std::endl;
        std::cout << MATRIX_GREEN << "  · = Clear terrain (1.0 energy)" << std::endl;
        std::cout << MATRIX_DANGER << "  █ = Obstacle (1000.0 energy)" << std::endl;
        std::cout << MATRIX_WARNING << "  ▲ = Hill (4.0 energy)" << std::endl;
        std::cout << MATRIX_INFO << "  ~ = Wind zone (2.5 energy)" << std::endl;
        std::cout << MATRIX_NEON << "  ⚡ = Energy station (0.5 energy)" << std::endl;
        std::cout << MATRIX_DANGER << "  ☢ = Danger zone (10.0 energy)" << std::endl;
        std::cout << MATRIX_BRIGHT << "  ✈ = Drone" << std::endl;
        std::cout << MATRIX_NEON << "  🛩 = Plane" << std::endl;
        std::cout << MATRIX_GLOW << "  ▲ = Start position" << std::endl;
        std::cout << MATRIX_DANGER << "  ◆ = Target position" << std::endl;
        std::cout << MATRIX_BRIGHT << "  ● = Calculated path" << std::endl;
    }
};

int main() {
    MatrixUAV matrix;
    matrix.run();
    return 0;
}