#ifndef MAP_PARSER_H
#define MAP_PARSER_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "Terrain.h"

class MapParser {
private:
    // Helper methods for parsing
    TerrainType charToTerrainType(char c) const;
    char terrainTypeToChar(TerrainType type) const;
    
    // File validation
    bool isValidMapFile(const std::string& filename) const;
    
public:
    MapParser();
    
    // Map loading methods
    Terrain loadMap(const std::string& filename);
    Terrain loadMapFromString(const std::string& mapData);
    
    // Map saving methods
    bool saveMap(const Terrain& terrain, const std::string& filename) const;
    std::string terrainToString(const Terrain& terrain) const;
    
    // Configuration loading
    bool loadConfiguration(const std::string& configFile);
    
    // Map generation
    Terrain generateRandomMap(int width, int height, double obstacleRatio = 0.2, 
                             double hillRatio = 0.1, double windRatio = 0.1);
    
    // Utility methods
    std::vector<std::string> parseMapLines(const std::string& content) const;
    bool validateMapDimensions(const std::vector<std::string>& lines) const;
    
    // Default maps
    Terrain createSampleMap();
    Terrain createComplexMap();
};

#endif
