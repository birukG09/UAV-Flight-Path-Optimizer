package com.uav.optimizer;

/**
 * Enumeration of different terrain types for UAV navigation
 */
public enum TerrainType {
    NORMAL,     // Standard terrain with normal movement cost
    OBSTACLE,   // Blocked terrain that should be avoided
    HILL,       // Elevated terrain with higher energy cost
    WIND        // Wind zones with moderate energy cost
}