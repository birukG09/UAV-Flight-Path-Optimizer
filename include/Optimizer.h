#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>
#include "Terrain.h"
#include "Drone.h"

struct PathNode {
    Point position;
    double gCost; // Distance from start
    double hCost; // Heuristic distance to goal
    double fCost; // Total cost (g + h)
    PathNode* parent;
    
    PathNode(const Point& pos, double g = 0, double h = 0, PathNode* p = nullptr)
        : position(pos), gCost(g), hCost(h), fCost(g + h), parent(p) {}
};

// Custom comparator for priority queue
struct PathNodeComparator {
    bool operator()(const PathNode* a, const PathNode* b) const {
        if (a->fCost != b->fCost) {
            return a->fCost > b->fCost; // Lower fCost has higher priority
        }
        return a->hCost > b->hCost; // If fCost is equal, prefer lower hCost
    }
};

class Optimizer {
private:
    const Terrain& terrain;
    
    // A* algorithm implementation
    std::vector<Point> reconstructPath(PathNode* goalNode);
    double calculateDistance(const Point& a, const Point& b) const;
    
    // Hash function for Point in unordered_map
    struct PointHash {
        size_t operator()(const Point& p) const {
            return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
        }
    };
    
public:
    Optimizer(const Terrain& terrainRef);
    
    // Main pathfinding methods
    std::vector<Point> findPath(const Point& start, const Point& goal);
    std::vector<Point> findPathAStar(const Point& start, const Point& goal);
    std::vector<Point> findPathDijkstra(const Point& start, const Point& goal);
    
    // Path optimization
    std::vector<Point> optimizePath(const std::vector<Point>& path);
    double calculatePathCost(const std::vector<Point>& path) const;
    
    // Utility methods
    bool isPathValid(const std::vector<Point>& path) const;
    void printPathStatistics(const std::vector<Point>& path) const;
    
    // Alternative algorithms for comparison
    std::vector<Point> findPathGreedy(const Point& start, const Point& goal);
    
    // Multi-objective optimization (energy + distance)
    std::vector<Point> findEnergyOptimalPath(const Point& start, const Point& goal, double energyWeight = 1.0);
};

#endif
