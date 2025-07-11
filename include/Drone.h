#ifndef DRONE_H
#define DRONE_H

#include <vector>

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Point& other) const {
        return !(*this == other);
    }
};

class Drone {
private:
    Point position;
    double maxEnergy;
    double currentEnergy;
    std::vector<Point> flightPath;
    
public:
    Drone(const Point& startPos, double maxEnergyCapacity);
    
    // Position management
    void setPosition(const Point& pos);
    Point getPosition() const;
    
    // Energy management
    double getMaxEnergy() const;
    double getCurrentEnergy() const;
    void consumeEnergy(double amount);
    void resetEnergy();
    bool hasEnergy(double required) const;
    
    // Flight path management
    void addToPath(const Point& point);
    std::vector<Point> getFlightPath() const;
    void clearPath();
    
    // Utility functions
    double getEnergyPercentage() const;
    bool isLowEnergy() const; // Below 20%
    void displayStatus() const;
};

#endif
