#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <string>
#include "Drone.h"

enum class TerrainType {
    NORMAL = 0,
    HILL = 1,
    OBSTACLE = 2,
    WIND_ZONE = 3,
    START = 4,
    END = 5
};

class Terrain {
private:
    std::vector<std::vector<TerrainType>> grid;
    std::vector<std::vector<double>> elevationMap;
    std::vector<std::vector<double>> windResistance;
    int width, height;
    
    // Cost calculation constants
    static constexpr double NORMAL_COST = 1.0;
    static constexpr double HILL_COST = 3.0;
    static constexpr double WIND_COST = 2.0;
    static constexpr double OBSTACLE_COST = 1000.0; // Effectively impassable
    
public:
    Terrain(int w, int h);
    
    // Grid management
    void setTerrain(int x, int y, TerrainType type);
    TerrainType getTerrain(int x, int y) const;
    void setElevation(int x, int y, double elevation);
    double getElevation(int x, int y) const;
    void setWindResistance(int x, int y, double resistance);
    double getWindResistance(int x, int y) const;
    
    // Validation
    bool isValidPosition(const Point& pos) const;
    bool isObstacle(const Point& pos) const;
    bool isPassable(const Point& pos) const;
    
    // Cost calculation
    double getMovementCost(const Point& pos) const;
    double getHeuristicCost(const Point& from, const Point& to) const;
    
    // Visualization
    void visualizeTerrain() const;
    void visualizePath(const std::vector<Point>& path) const;
    char getTerrainChar(TerrainType type) const;
    
    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    // Neighbors for pathfinding
    std::vector<Point> getNeighbors(const Point& pos) const;
    
    // Terrain generation
    void generateRandomTerrain(double obstacleProb = 0.2, double hillProb = 0.1, double windProb = 0.1);
    void addObstacle(const Point& pos);
    void addHill(const Point& pos);
    void addWindZone(const Point& pos);
};

#endif
