package com.uav.optimizer;

/**
 * Represents a 2D grid of terrain types for UAV navigation
 */
public class TerrainGrid {
    private TerrainType[][] grid;
    private int width;
    private int height;
    
    public TerrainGrid(int width, int height) {
        this.width = width;
        this.height = height;
        this.grid = new TerrainType[width][height];
        
        // Initialize with normal terrain
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                grid[x][y] = TerrainType.NORMAL;
            }
        }
    }
    
    public int getWidth() {
        return width;
    }
    
    public int getHeight() {
        return height;
    }
    
    public TerrainType getCell(int x, int y) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            return grid[x][y];
        }
        return TerrainType.OBSTACLE; // Out of bounds treated as obstacle
    }
    
    public void setCell(int x, int y, TerrainType type) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[x][y] = type;
        }
    }
    
    public boolean isValidPosition(int x, int y) {
        return x >= 0 && x < width && y >= 0 && y < height && 
               grid[x][y] != TerrainType.OBSTACLE;
    }
    
    public double getMovementCost(int x, int y) {
        TerrainType type = getCell(x, y);
        switch (type) {
            case NORMAL: return 1.0;
            case HILL: return 3.0;
            case WIND: return 2.0;
            case OBSTACLE: return 1000.0; // Very high cost to discourage
            default: return 1.0;
        }
    }
    
    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                sb.append(getTerrainChar(grid[x][y]));
            }
            sb.append('\n');
        }
        return sb.toString();
    }
    
    private char getTerrainChar(TerrainType type) {
        switch (type) {
            case OBSTACLE: return 'O';
            case HILL: return '^';
            case WIND: return 'W';
            default: return '.';
        }
    }
}