use crate::terrain::{Terrain, TerrainType};
use std::fs;

pub struct MapParser {
}

impl MapParser {
    pub fn new() -> Self {
        MapParser {}
    }

    pub fn load_map(&self, filename: &str) -> Result<Terrain, String> {
        let content = fs::read_to_string(filename)
            .map_err(|e| format!("Could not open map file: {} - {}", filename, e))?;
        
        self.load_map_from_string(&content)
    }

    pub fn load_map_from_string(&self, map_data: &str) -> Result<Terrain, String> {
        let lines = self.parse_map_lines(map_data);
        
        if lines.is_empty() {
            return Err("Empty map data".to_string());
        }
        
        if !self.validate_map_dimensions(&lines) {
            return Err("Invalid map dimensions - all rows must have same length".to_string());
        }
        
        let height = lines.len();
        let width = lines[0].len();
        
        let mut terrain = Terrain::new(width, height);
        
        for (y, line) in lines.iter().enumerate() {
            for (x, ch) in line.chars().enumerate() {
                let terrain_type = self.char_to_terrain_type(ch);
                terrain.set_terrain(x, y, terrain_type);
                
                // Set additional properties based on terrain type
                match terrain_type {
                    TerrainType::Hill => {
                        terrain.set_elevation(x, y, 3.0);
                    }
                    TerrainType::WindZone => {
                        terrain.set_wind_resistance(x, y, 2.0);
                    }
                    _ => {}
                }
            }
        }
        
        Ok(terrain)
    }

    pub fn save_map(&self, terrain: &Terrain, filename: &str) -> Result<(), String> {
        let map_string = self.terrain_to_string(terrain);
        fs::write(filename, map_string)
            .map_err(|e| format!("Could not save map file: {} - {}", filename, e))
    }

    pub fn terrain_to_string(&self, terrain: &Terrain) -> String {
        let mut result = String::new();
        
        for y in 0..terrain.get_height() {
            for x in 0..terrain.get_width() {
                let terrain_type = terrain.get_terrain(x, y);
                result.push(self.terrain_type_to_char(terrain_type));
            }
            result.push('\n');
        }
        
        result
    }

    pub fn create_sample_map(&self) -> Terrain {
        let sample_map_data = 
            "...........\n\
             ..O.O.O....\n\
             ...........\n\
             .O..^..O...\n\
             ...........\n\
             ...W.W.W...\n\
             ...........\n\
             .O..^..O...\n\
             ...........\n\
             ..O.O.O....\n\
             ...........\n";
        
        self.load_map_from_string(sample_map_data).unwrap()
    }

    pub fn create_complex_map(&self) -> Terrain {
        let complex_map_data = 
            "..O.......O.......O..\n\
             .O..^^^^^..O.WWW.O...\n\
             O....^^^....W.W.W...O\n\
             .....^.^.....W.W.....\n\
             ..O...^...O...W...O..\n\
             ......^..............\n\
             ..OOO.^.OOO.WWW.OOO..\n\
             ......^..............\n\
             ..O...^...O...W...O..\n\
             .....^.^.....W.W.....\n\
             O....^^^....W.W.W...O\n\
             .O..^^^^^..O.WWW.O...\n\
             ..O.......O.......O..\n";
        
        self.load_map_from_string(complex_map_data).unwrap()
    }

    fn char_to_terrain_type(&self, c: char) -> TerrainType {
        match c {
            '.' => TerrainType::Normal,
            '^' => TerrainType::Hill,
            'O' | 'o' => TerrainType::Obstacle,
            'W' | 'w' => TerrainType::WindZone,
            'S' | 's' => TerrainType::Start,
            'D' | 'd' => TerrainType::End,
            _ => TerrainType::Normal,
        }
    }

    fn terrain_type_to_char(&self, terrain_type: TerrainType) -> char {
        match terrain_type {
            TerrainType::Normal => '.',
            TerrainType::Hill => '^',
            TerrainType::Obstacle => 'O',
            TerrainType::WindZone => 'W',
            TerrainType::Start => 'S',
            TerrainType::End => 'D',
        }
    }

    fn parse_map_lines(&self, content: &str) -> Vec<String> {
        content
            .lines()
            .map(|line| line.trim_end_matches('\r').to_string())
            .filter(|line| !line.is_empty())
            .collect()
    }

    fn validate_map_dimensions(&self, lines: &Vec<String>) -> bool {
        if lines.is_empty() {
            return false;
        }
        
        let expected_width = lines[0].len();
        lines.iter().all(|line| line.len() == expected_width)
    }
}