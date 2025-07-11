#include "../include/MapParser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <random>

MapParser::MapParser() {}

TerrainType MapParser::charToTerrainType(char c) const {
    switch (c) {
        case '.': return TerrainType::NORMAL;
        case '^': return TerrainType::HILL;
        case 'O': case 'o': return TerrainType::OBSTACLE;
        case 'W': case 'w': return TerrainType::WIND_ZONE;
        case 'S': case 's': return TerrainType::START;
        case 'D': case 'd': return TerrainType::END;
        default: return TerrainType::NORMAL;
    }
}

char MapParser::terrainTypeToChar(TerrainType type) const {
    switch (type) {
        case TerrainType::NORMAL: return '.';
        case TerrainType::HILL: return '^';
        case TerrainType::OBSTACLE: return 'O';
        case TerrainType::WIND_ZONE: return 'W';
        case TerrainType::START: return 'S';
        case TerrainType::END: return 'D';
        default: return '.';
    }
}

bool MapParser::isValidMapFile(const std::string& filename) const {
    std::ifstream file(filename);
    return file.good();
}

Terrain MapParser::loadMap(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open map file: " + filename);
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    return loadMapFromString(content);
}

Terrain MapParser::loadMapFromString(const std::string& mapData) {
    std::vector<std::string> lines = parseMapLines(mapData);
    
    if (lines.empty()) {
        throw std::runtime_error("Empty map data");
    }
    
    if (!validateMapDimensions(lines)) {
        throw std::runtime_error("Invalid map dimensions - all rows must have same length");
    }
    
    int height = lines.size();
    int width = lines[0].length();
    
    Terrain terrain(width, height);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (x < static_cast<int>(lines[y].length())) {
                TerrainType type = charToTerrainType(lines[y][x]);
                terrain.setTerrain(x, y, type);
                
                // Set additional properties based on terrain type
                switch (type) {
                    case TerrainType::HILL:
                        terrain.setElevation(x, y, 3.0);
                        break;
                    case TerrainType::WIND_ZONE:
                        terrain.setWindResistance(x, y, 2.0);
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
    return terrain;
}

bool MapParser::saveMap(const Terrain& terrain, const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << terrainToString(terrain);
    file.close();
    return true;
}

std::string MapParser::terrainToString(const Terrain& terrain) const {
    std::stringstream ss;
    
    for (int y = 0; y < terrain.getHeight(); y++) {
        for (int x = 0; x < terrain.getWidth(); x++) {
            TerrainType type = terrain.getTerrain(x, y);
            ss << terrainTypeToChar(type);
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::vector<std::string> MapParser::parseMapLines(const std::string& content) const {
    std::vector<std::string> lines;
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Remove carriage return if present
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    
    return lines;
}

bool MapParser::validateMapDimensions(const std::vector<std::string>& lines) const {
    if (lines.empty()) return false;
    
    size_t expectedWidth = lines[0].length();
    for (const std::string& line : lines) {
        if (line.length() != expectedWidth) {
            return false;
        }
    }
    
    return true;
}

Terrain MapParser::generateRandomMap(int width, int height, double obstacleRatio, 
                                   double hillRatio, double windRatio) {
    Terrain terrain(width, height);
    terrain.generateRandomTerrain(obstacleRatio, hillRatio, windRatio);
    return terrain;
}

Terrain MapParser::createSampleMap() {
    std::string sampleMapData = 
        "...........\n"
        "..O.O.O....\n"
        "...........\n"
        ".O..^..O...\n"
        "...........\n"
        "...W.W.W...\n"
        "...........\n"
        ".O..^..O...\n"
        "...........\n"
        "..O.O.O....\n"
        "...........\n";
    
    return loadMapFromString(sampleMapData);
}

Terrain MapParser::createComplexMap() {
    std::string complexMapData = 
        "..O.......O.......O..\n"
        ".O..^^^^^..O.WWW.O...\n"
        "O....^^^....W.W.W...O\n"
        ".....^.^.....W.W.....\n"
        "..O...^...O...W...O..\n"
        "......^..............\n"
        "..OOO.^.OOO.WWW.OOO..\n"
        "......^..............\n"
        "..O...^...O...W...O..\n"
        ".....^.^.....W.W.....\n"
        "O....^^^....W.W.W...O\n"
        ".O..^^^^^..O.WWW.O...\n"
        "..O.......O.......O..\n";
    
    return loadMapFromString(complexMapData);
}

bool MapParser::loadConfiguration(const std::string& configFile) {
    std::ifstream file(configFile);
    if (!file.is_open()) {
        std::cout << "Warning: Could not load configuration file: " << configFile << "\n";
        return false;
    }
    
    // Simple JSON-like parsing for basic configuration
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#' || line[0] == '/') continue;
        
        // Basic key-value parsing
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            
            // Remove whitespace
            key.erase(std::remove_if(key.begin(), key.end(), ::isspace), key.end());
            value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());
            
            // Remove quotes
            if (value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.length() - 2);
            }
            
            std::cout << "Config: " << key << " = " << value << "\n";
        }
    }
    
    file.close();
    return true;
}
