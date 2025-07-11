package com.uav.optimizer;

/**
 * Simple Point class for representing coordinates in 2D space
 */
public class Point {
    public int x;
    public int y;
    
    public Point(int x, int y) {
        this.x = x;
        this.y = y;
    }
    
    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Point point = (Point) obj;
        return x == point.x && y == point.y;
    }
    
    @Override
    public int hashCode() {
        return x * 31 + y;
    }
    
    @Override
    public String toString() {
        return "(" + x + ", " + y + ")";
    }
}