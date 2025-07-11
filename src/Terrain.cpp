#include "../include/Terrain.h"
#include <iostream>
#include <random>
#include <iomanip>
#include <algorithm>
#include <cmath>

// ANSI color codes for green terminal output
#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define BRIGHT_GREEN "\033[1;32m"

Terrain::Terrain(int w, int h) : width(w), height(h) {
    grid.resize(height, std::vector<TerrainType>(width, TerrainType::NORMAL));
    elevationMap.resize(height, std::vector<double>(width, 0.0));
    windResistance.resize(height, std::vector<double>(width, 0.0));
}

void Terrain::setTerrain(int x, int y, TerrainType type) {
    if (isValidPosition(Point(x, y))) {
        grid[y][x] = type;
    }
}

TerrainType Terrain::getTerrain(int x, int y) const {
    if (isValidPosition(Point(x, y))) {
        return grid[y][x];
    }
    return TerrainType::OBSTACLE;
}

void Terrain::setElevation(int x, int y, double elevation) {
    if (isValidPosition(Point(x, y))) {
        elevationMap[y][x] = elevation;
    }
}

double Terrain::getElevation(int x, int y) const {
    if (isValidPosition(Point(x, y))) {
        return elevationMap[y][x];
    }
    return 0.0;
}

void Terrain::setWindResistance(int x, int y, double resistance) {
    if (isValidPosition(Point(x, y))) {
        windResistance[y][x] = resistance;
    }
}

double Terrain::getWindResistance(int x, int y) const {
    if (isValidPosition(Point(x, y))) {
        return windResistance[y][x];
    }
    return 0.0;
}

bool Terrain::isValidPosition(const Point& pos) const {
    return pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height;
}

bool Terrain::isObstacle(const Point& pos) const {
    if (!isValidPosition(pos)) return true;
    return grid[pos.y][pos.x] == TerrainType::OBSTACLE;
}

bool Terrain::isPassable(const Point& pos) const {
    return isValidPosition(pos) && !isObstacle(pos);
}

double Terrain::getMovementCost(const Point& pos) const {
    if (!isValidPosition(pos)) return OBSTACLE_COST;
    
    double baseCost = NORMAL_COST;
    TerrainType type = grid[pos.y][pos.x];
    
    switch (type) {
        case TerrainType::NORMAL:
            baseCost = NORMAL_COST;
            break;
        case TerrainType::HILL:
            baseCost = HILL_COST;
            break;
        case TerrainType::WIND_ZONE:
            baseCost = WIND_COST;
            break;
        case TerrainType::OBSTACLE:
            return OBSTACLE_COST;
        default:
            baseCost = NORMAL_COST;
    }
    
    // Add elevation cost
    double elevationCost = elevationMap[pos.y][pos.x] * 0.5;
    
    // Add wind resistance
    double windCost = windResistance[pos.y][pos.x] * 0.3;
    
    return baseCost + elevationCost + windCost;
}

double Terrain::getHeuristicCost(const Point& from, const Point& to) const {
    // Manhattan distance with diagonal movement consideration
    double dx = std::abs(to.x - from.x);
    double dy = std::abs(to.y - from.y);
    
    // Euclidean distance for more accurate heuristic
    return std::sqrt(dx * dx + dy * dy);
}

void Terrain::visualizeTerrain() const {
    std::cout << "=== Terrain Map ===\n";
    std::cout << "  ";
    for (int x = 0; x < width; x++) {
        std::cout << std::setw(2) << x;
    }
    std::cout << "\n";
    
    for (int y = 0; y < height; y++) {
        std::cout << std::setw(2) << y;
        for (int x = 0; x < width; x++) {
            std::cout << " " << getTerrainChar(grid[y][x]);
        }
        std::cout << "\n";
    }
    std::cout << "==================\n";
}

void Terrain::visualizePath(const std::vector<Point>& path) const {
    std::cout << GREEN << "  ";
    for (int x = 0; x < width; x++) {
        std::cout << std::setw(2) << x;
    }
    std::cout << RESET << "\n";
    
    for (int y = 0; y < height; y++) {
        std::cout << GREEN << std::setw(2) << y << RESET;
        for (int x = 0; x < width; x++) {
            Point current(x, y);
            bool isOnPath = false;
            
            // Check if this position is on the path
            for (const Point& pathPoint : path) {
                if (pathPoint == current) {
                    isOnPath = true;
                    break;
                }
            }
            
            char displayChar;
            if (isOnPath) {
                // Check if it's start or end
                if (current == path.front()) {
                    displayChar = 'S';
                } else if (current == path.back()) {
                    displayChar = 'D';
                } else {
                    displayChar = '*';  // Use * instead of Unicode character
                }
                std::cout << " " << BRIGHT_GREEN << displayChar << RESET;
            } else {
                displayChar = getTerrainChar(grid[y][x]);
                std::cout << " " << GREEN << displayChar << RESET;
            }
        }
        std::cout << "\n";
    }
}

char Terrain::getTerrainChar(TerrainType type) const {
    switch (type) {
        case TerrainType::NORMAL: return '.';
        case TerrainType::HILL: return '^';
        case TerrainType::OBSTACLE: return 'O';
        case TerrainType::WIND_ZONE: return 'W';
        case TerrainType::START: return 'S';
        case TerrainType::END: return 'D';
        default: return '?';
    }
}

std::vector<Point> Terrain::getNeighbors(const Point& pos) const {
    std::vector<Point> neighbors;
    
    // 8-directional movement (including diagonals)
    const int dx[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const int dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};
    
    for (int i = 0; i < 8; i++) {
        Point neighbor(pos.x + dx[i], pos.y + dy[i]);
        if (isPassable(neighbor)) {
            neighbors.push_back(neighbor);
        }
    }
    
    return neighbors;
}

void Terrain::generateRandomTerrain(double obstacleProb, double hillProb, double windProb) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double roll = dis(gen);
            
            if (roll < obstacleProb) {
                setTerrain(x, y, TerrainType::OBSTACLE);
            } else if (roll < obstacleProb + hillProb) {
                setTerrain(x, y, TerrainType::HILL);
                setElevation(x, y, dis(gen) * 5.0); // Random elevation 0-5
            } else if (roll < obstacleProb + hillProb + windProb) {
                setTerrain(x, y, TerrainType::WIND_ZONE);
                setWindResistance(x, y, dis(gen) * 3.0); // Random wind resistance 0-3
            } else {
                setTerrain(x, y, TerrainType::NORMAL);
                setElevation(x, y, dis(gen) * 2.0); // Small elevation variation
            }
        }
    }
}

void Terrain::addObstacle(const Point& pos) {
    setTerrain(pos.x, pos.y, TerrainType::OBSTACLE);
}

void Terrain::addHill(const Point& pos) {
    setTerrain(pos.x, pos.y, TerrainType::HILL);
    setElevation(pos.x, pos.y, 3.0);
}

void Terrain::addWindZone(const Point& pos) {
    setTerrain(pos.x, pos.y, TerrainType::WIND_ZONE);
    setWindResistance(pos.x, pos.y, 2.0);
}
