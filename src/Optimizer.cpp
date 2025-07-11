#include "../include/Optimizer.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <iomanip>

Optimizer::Optimizer(const Terrain& terrainRef) : terrain(terrainRef) {}

std::vector<Point> Optimizer::findPath(const Point& start, const Point& goal) {
    return findPathAStar(start, goal);
}

std::vector<Point> Optimizer::findPathAStar(const Point& start, const Point& goal) {
    std::priority_queue<PathNode*, std::vector<PathNode*>, PathNodeComparator> openSet;
    std::unordered_map<Point, PathNode*, PointHash> allNodes;
    std::unordered_map<Point, double, PointHash> gScore;
    
    // Initialize start node
    PathNode* startNode = new PathNode(start, 0, terrain.getHeuristicCost(start, goal));
    openSet.push(startNode);
    allNodes[start] = startNode;
    gScore[start] = 0;
    
    std::vector<PathNode*> createdNodes; // For cleanup
    createdNodes.push_back(startNode);
    
    while (!openSet.empty()) {
        PathNode* current = openSet.top();
        openSet.pop();
        
        // Check if we reached the goal
        if (current->position == goal) {
            std::vector<Point> path = reconstructPath(current);
            
            // Cleanup
            for (PathNode* node : createdNodes) {
                delete node;
            }
            
            return path;
        }
        
        // Explore neighbors
        std::vector<Point> neighbors = terrain.getNeighbors(current->position);
        
        for (const Point& neighbor : neighbors) {
            double movementCost = terrain.getMovementCost(neighbor);
            
            // Skip if obstacle
            if (movementCost >= 1000.0) continue;
            
            // Calculate diagonal movement cost
            double distance = calculateDistance(current->position, neighbor);
            double tentativeGScore = current->gCost + movementCost * distance;
            
            // Check if this path to neighbor is better
            if (gScore.find(neighbor) == gScore.end() || tentativeGScore < gScore[neighbor]) {
                gScore[neighbor] = tentativeGScore;
                
                double hCost = terrain.getHeuristicCost(neighbor, goal);
                PathNode* neighborNode = new PathNode(neighbor, tentativeGScore, hCost, current);
                
                allNodes[neighbor] = neighborNode;
                openSet.push(neighborNode);
                createdNodes.push_back(neighborNode);
            }
        }
    }
    
    // Cleanup if no path found
    for (PathNode* node : createdNodes) {
        delete node;
    }
    
    return std::vector<Point>(); // Empty path = no solution
}

std::vector<Point> Optimizer::findPathDijkstra(const Point& start, const Point& goal) {
    std::priority_queue<PathNode*, std::vector<PathNode*>, PathNodeComparator> openSet;
    std::unordered_map<Point, PathNode*, PointHash> allNodes;
    std::unordered_map<Point, double, PointHash> distances;
    
    // Initialize start node (no heuristic for Dijkstra)
    PathNode* startNode = new PathNode(start, 0, 0);
    openSet.push(startNode);
    allNodes[start] = startNode;
    distances[start] = 0;
    
    std::vector<PathNode*> createdNodes;
    createdNodes.push_back(startNode);
    
    while (!openSet.empty()) {
        PathNode* current = openSet.top();
        openSet.pop();
        
        if (current->position == goal) {
            std::vector<Point> path = reconstructPath(current);
            
            for (PathNode* node : createdNodes) {
                delete node;
            }
            
            return path;
        }
        
        std::vector<Point> neighbors = terrain.getNeighbors(current->position);
        
        for (const Point& neighbor : neighbors) {
            double movementCost = terrain.getMovementCost(neighbor);
            if (movementCost >= 1000.0) continue;
            
            double distance = calculateDistance(current->position, neighbor);
            double newDistance = current->gCost + movementCost * distance;
            
            if (distances.find(neighbor) == distances.end() || newDistance < distances[neighbor]) {
                distances[neighbor] = newDistance;
                
                PathNode* neighborNode = new PathNode(neighbor, newDistance, 0, current);
                allNodes[neighbor] = neighborNode;
                openSet.push(neighborNode);
                createdNodes.push_back(neighborNode);
            }
        }
    }
    
    for (PathNode* node : createdNodes) {
        delete node;
    }
    
    return std::vector<Point>();
}

std::vector<Point> Optimizer::reconstructPath(PathNode* goalNode) {
    std::vector<Point> path;
    PathNode* current = goalNode;
    
    while (current != nullptr) {
        path.push_back(current->position);
        current = current->parent;
    }
    
    std::reverse(path.begin(), path.end());
    return path;
}

double Optimizer::calculateDistance(const Point& a, const Point& b) const {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

std::vector<Point> Optimizer::optimizePath(const std::vector<Point>& path) {
    if (path.size() <= 2) return path;
    
    std::vector<Point> optimized;
    optimized.push_back(path[0]);
    
    for (size_t i = 1; i < path.size() - 1; i++) {
        Point prev = path[i - 1];
        Point current = path[i];
        Point next = path[i + 1];
        
        // Check if we can skip current point by going directly from prev to next
        bool canSkip = true;
        
        // Simple line-of-sight check
        int dx = next.x - prev.x;
        int dy = next.y - prev.y;
        int steps = std::max(std::abs(dx), std::abs(dy));
        
        if (steps > 0) {
            for (int step = 1; step < steps; step++) {
                int x = prev.x + (dx * step) / steps;
                int y = prev.y + (dy * step) / steps;
                
                if (terrain.isObstacle(Point(x, y))) {
                    canSkip = false;
                    break;
                }
            }
        }
        
        if (!canSkip) {
            optimized.push_back(current);
        }
    }
    
    optimized.push_back(path.back());
    return optimized;
}

double Optimizer::calculatePathCost(const std::vector<Point>& path) const {
    if (path.empty()) return 0.0;
    
    double totalCost = 0.0;
    for (size_t i = 0; i < path.size(); i++) {
        totalCost += terrain.getMovementCost(path[i]);
    }
    
    return totalCost;
}

bool Optimizer::isPathValid(const std::vector<Point>& path) const {
    for (const Point& point : path) {
        if (!terrain.isPassable(point)) {
            return false;
        }
    }
    return true;
}

void Optimizer::printPathStatistics(const std::vector<Point>& path) const {
    if (path.empty()) {
        std::cout << "No path found!\n";
        return;
    }
    
    double totalCost = calculatePathCost(path);
    double totalDistance = 0.0;
    
    for (size_t i = 1; i < path.size(); i++) {
        totalDistance += calculateDistance(path[i-1], path[i]);
    }
    
    std::cout << "=== Path Statistics ===\n";
    std::cout << "Path Length: " << path.size() << " steps\n";
    std::cout << "Total Distance: " << std::fixed << std::setprecision(2) << totalDistance << " units\n";
    std::cout << "Total Cost: " << std::fixed << std::setprecision(2) << totalCost << " energy units\n";
    std::cout << "Average Cost per Step: " << std::fixed << std::setprecision(2) << totalCost / path.size() << "\n";
    std::cout << "Path Valid: " << (isPathValid(path) ? "Yes" : "No") << "\n";
    std::cout << "======================\n";
}

std::vector<Point> Optimizer::findPathGreedy(const Point& start, const Point& goal) {
    std::vector<Point> path;
    Point current = start;
    path.push_back(current);
    
    while (current != goal) {
        std::vector<Point> neighbors = terrain.getNeighbors(current);
        
        if (neighbors.empty()) break;
        
        // Find neighbor with minimum heuristic cost to goal
        Point bestNeighbor = neighbors[0];
        double bestCost = terrain.getHeuristicCost(bestNeighbor, goal);
        
        for (const Point& neighbor : neighbors) {
            double cost = terrain.getHeuristicCost(neighbor, goal);
            if (cost < bestCost) {
                bestCost = cost;
                bestNeighbor = neighbor;
            }
        }
        
        current = bestNeighbor;
        path.push_back(current);
        
        // Prevent infinite loops
        if (path.size() > static_cast<size_t>(terrain.getWidth() * terrain.getHeight())) {
            break;
        }
    }
    
    return path;
}

std::vector<Point> Optimizer::findEnergyOptimalPath(const Point& start, const Point& goal, double energyWeight) {
    std::priority_queue<PathNode*, std::vector<PathNode*>, PathNodeComparator> openSet;
    std::unordered_map<Point, PathNode*, PointHash> allNodes;
    std::unordered_map<Point, double, PointHash> gScore;
    
    PathNode* startNode = new PathNode(start, 0, terrain.getHeuristicCost(start, goal));
    openSet.push(startNode);
    allNodes[start] = startNode;
    gScore[start] = 0;
    
    std::vector<PathNode*> createdNodes;
    createdNodes.push_back(startNode);
    
    while (!openSet.empty()) {
        PathNode* current = openSet.top();
        openSet.pop();
        
        if (current->position == goal) {
            std::vector<Point> path = reconstructPath(current);
            
            for (PathNode* node : createdNodes) {
                delete node;
            }
            
            return path;
        }
        
        std::vector<Point> neighbors = terrain.getNeighbors(current->position);
        
        for (const Point& neighbor : neighbors) {
            double movementCost = terrain.getMovementCost(neighbor);
            if (movementCost >= 1000.0) continue;
            
            double distance = calculateDistance(current->position, neighbor);
            
            // Energy-weighted cost function
            double energyCost = movementCost * energyWeight;
            double tentativeGScore = current->gCost + energyCost * distance;
            
            if (gScore.find(neighbor) == gScore.end() || tentativeGScore < gScore[neighbor]) {
                gScore[neighbor] = tentativeGScore;
                
                double hCost = terrain.getHeuristicCost(neighbor, goal);
                PathNode* neighborNode = new PathNode(neighbor, tentativeGScore, hCost, current);
                
                allNodes[neighbor] = neighborNode;
                openSet.push(neighborNode);
                createdNodes.push_back(neighborNode);
            }
        }
    }
    
    for (PathNode* node : createdNodes) {
        delete node;
    }
    
    return std::vector<Point>();
}
