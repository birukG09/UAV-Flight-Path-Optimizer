#include "../include/Drone.h"
#include <iostream>
#include <iomanip>

Drone::Drone(const Point& startPos, double maxEnergyCapacity) 
    : position(startPos), maxEnergy(maxEnergyCapacity), currentEnergy(maxEnergyCapacity) {
    flightPath.clear();
    addToPath(startPos);
}

void Drone::setPosition(const Point& pos) {
    position = pos;
}

Point Drone::getPosition() const {
    return position;
}

double Drone::getMaxEnergy() const {
    return maxEnergy;
}

double Drone::getCurrentEnergy() const {
    return currentEnergy;
}

void Drone::consumeEnergy(double amount) {
    currentEnergy = std::max(0.0, currentEnergy - amount);
}

void Drone::resetEnergy() {
    currentEnergy = maxEnergy;
}

bool Drone::hasEnergy(double required) const {
    return currentEnergy >= required;
}

void Drone::addToPath(const Point& point) {
    flightPath.push_back(point);
}

std::vector<Point> Drone::getFlightPath() const {
    return flightPath;
}

void Drone::clearPath() {
    flightPath.clear();
}

double Drone::getEnergyPercentage() const {
    return (currentEnergy / maxEnergy) * 100.0;
}

bool Drone::isLowEnergy() const {
    return getEnergyPercentage() < 20.0;
}

void Drone::displayStatus() const {
    std::cout << "=== Drone Status ===\n";
    std::cout << "Position: (" << position.x << ", " << position.y << ")\n";
    std::cout << "Energy: " << std::fixed << std::setprecision(1) 
              << currentEnergy << "/" << maxEnergy 
              << " (" << getEnergyPercentage() << "%)\n";
    std::cout << "Path Length: " << flightPath.size() << " waypoints\n";
    
    if (isLowEnergy()) {
        std::cout << "WARNING: Low energy level!\n";
    }
    
    std::cout << "===================\n";
}
