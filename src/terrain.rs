use std::collections::HashMap;

// ANSI color codes for green terminal output
const RESET: &str = "\x1b[0m";
const GREEN: &str = "\x1b[32m";
const BRIGHT_GREEN: &str = "\x1b[1;32m";

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Point {
    pub x: i32,
    pub y: i32,
}

impl Point {
    pub fn new(x: i32, y: i32) -> Self {
        Point { x, y }
    }
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum TerrainType {
    Normal,
    Hill,
    Obstacle,
    WindZone,
    Start,
    End,
}

pub struct Terrain {
    grid: Vec<Vec<TerrainType>>,
    elevation_map: Vec<Vec<f64>>,
    wind_resistance: Vec<Vec<f64>>,
    width: usize,
    height: usize,
}

impl Terrain {
    pub fn new(width: usize, height: usize) -> Self {
        Terrain {
            grid: vec![vec![TerrainType::Normal; width]; height],
            elevation_map: vec![vec![0.0; width]; height],
            wind_resistance: vec![vec![0.0; width]; height],
            width,
            height,
        }
    }

    pub fn set_terrain(&mut self, x: usize, y: usize, terrain_type: TerrainType) {
        if self.is_valid_position(&Point::new(x as i32, y as i32)) {
            self.grid[y][x] = terrain_type;
        }
    }

    pub fn get_terrain(&self, x: usize, y: usize) -> TerrainType {
        if self.is_valid_position(&Point::new(x as i32, y as i32)) {
            self.grid[y][x]
        } else {
            TerrainType::Obstacle
        }
    }

    pub fn set_elevation(&mut self, x: usize, y: usize, elevation: f64) {
        if self.is_valid_position(&Point::new(x as i32, y as i32)) {
            self.elevation_map[y][x] = elevation;
        }
    }

    pub fn get_elevation(&self, x: usize, y: usize) -> f64 {
        if self.is_valid_position(&Point::new(x as i32, y as i32)) {
            self.elevation_map[y][x]
        } else {
            0.0
        }
    }

    pub fn set_wind_resistance(&mut self, x: usize, y: usize, resistance: f64) {
        if self.is_valid_position(&Point::new(x as i32, y as i32)) {
            self.wind_resistance[y][x] = resistance;
        }
    }

    pub fn get_wind_resistance(&self, x: usize, y: usize) -> f64 {
        if self.is_valid_position(&Point::new(x as i32, y as i32)) {
            self.wind_resistance[y][x]
        } else {
            0.0
        }
    }

    pub fn is_valid_position(&self, pos: &Point) -> bool {
        pos.x >= 0 && pos.x < self.width as i32 && pos.y >= 0 && pos.y < self.height as i32
    }

    pub fn is_obstacle(&self, pos: &Point) -> bool {
        if !self.is_valid_position(pos) {
            return true;
        }
        matches!(self.grid[pos.y as usize][pos.x as usize], TerrainType::Obstacle)
    }

    pub fn is_passable(&self, pos: &Point) -> bool {
        self.is_valid_position(pos) && !self.is_obstacle(pos)
    }

    pub fn get_movement_cost(&self, pos: &Point) -> f64 {
        if !self.is_valid_position(pos) {
            return 1000.0;
        }

        let x = pos.x as usize;
        let y = pos.y as usize;
        
        let base_cost = match self.grid[y][x] {
            TerrainType::Normal => 1.0,
            TerrainType::Hill => 3.0,
            TerrainType::WindZone => 2.0,
            TerrainType::Obstacle => 1000.0,
            _ => 1.0,
        };

        if base_cost >= 1000.0 {
            return base_cost;
        }

        // Add elevation cost
        let elevation_cost = self.elevation_map[y][x] * 0.5;
        
        // Add wind resistance
        let wind_cost = self.wind_resistance[y][x] * 0.3;
        
        base_cost + elevation_cost + wind_cost
    }

    pub fn get_heuristic_cost(&self, from: &Point, to: &Point) -> f64 {
        let dx = (to.x - from.x) as f64;
        let dy = (to.y - from.y) as f64;
        (dx * dx + dy * dy).sqrt()
    }

    pub fn get_neighbors(&self, pos: &Point) -> Vec<Point> {
        let mut neighbors = Vec::new();
        
        // 8-directional movement (including diagonals)
        let directions = [
            (-1, -1), (-1, 0), (-1, 1),
            (0, -1),           (0, 1),
            (1, -1),  (1, 0),  (1, 1)
        ];
        
        for (dx, dy) in directions {
            let neighbor = Point::new(pos.x + dx, pos.y + dy);
            if self.is_passable(&neighbor) {
                neighbors.push(neighbor);
            }
        }
        
        neighbors
    }

    pub fn visualize_path(&self, path: &Vec<Point>) {
        // Create a HashMap for quick path lookup
        let path_set: HashMap<Point, usize> = path.iter().enumerate().map(|(i, &p)| (p, i)).collect();
        
        // Print column headers
        print!("{}  {}", GREEN, RESET);
        for x in 0..self.width {
            print!("{}{:2}{}", GREEN, x, RESET);
        }
        println!();
        
        // Print grid with path
        for y in 0..self.height {
            print!("{}{:2}{}", GREEN, y, RESET);
            for x in 0..self.width {
                let current = Point::new(x as i32, y as i32);
                
                if let Some(&path_index) = path_set.get(&current) {
                    // This position is on the path
                    let display_char = if path_index == 0 {
                        'S'
                    } else if path_index == path.len() - 1 {
                        'D'
                    } else {
                        '*'
                    };
                    print!(" {}{}{}", BRIGHT_GREEN, display_char, RESET);
                } else {
                    // Regular terrain
                    let terrain_char = self.get_terrain_char(self.grid[y][x]);
                    print!(" {}{}{}", GREEN, terrain_char, RESET);
                }
            }
            println!();
        }
    }

    fn get_terrain_char(&self, terrain_type: TerrainType) -> char {
        match terrain_type {
            TerrainType::Normal => '.',
            TerrainType::Hill => '^',
            TerrainType::Obstacle => 'O',
            TerrainType::WindZone => 'W',
            TerrainType::Start => 'S',
            TerrainType::End => 'D',
        }
    }

    pub fn get_width(&self) -> usize {
        self.width
    }

    pub fn get_height(&self) -> usize {
        self.height
    }
}