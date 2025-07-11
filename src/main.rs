use std::env;
use std::fs;
use std::time::Instant;

// ANSI color codes for green terminal output
const RESET: &str = "\x1b[0m";
const GREEN: &str = "\x1b[32m";
const BRIGHT_GREEN: &str = "\x1b[1;32m";

mod terrain;
mod drone;
mod optimizer;
mod map_parser;

use terrain::{Terrain, Point};
use drone::Drone;
use optimizer::Optimizer;
use map_parser::MapParser;

fn print_usage() {
    println!("{}Usage: ./uav_optimizer [map_file] [start_x] [start_y] [end_x] [end_y]{}", GREEN, RESET);
    println!("{}Example: ./uav_optimizer maps/sample_map.txt 0 0 9 9{}", GREEN, RESET);
    println!("{}If no arguments provided, default sample will be used.{}", GREEN, RESET);
}

fn display_results(
    path: &Vec<Point>,
    terrain: &Terrain,
    _drone: &Drone,
    total_time: f64,
    total_energy: f64,
) {
    println!("\n{}{}{}", BRIGHT_GREEN, "=== UAV Flight Path Optimization Results ===", RESET);
    println!("{}{}{}", BRIGHT_GREEN, "Algorithm: A* Pathfinding", RESET);
    println!("{}Computation Time: {:.3} seconds{}", GREEN, total_time, RESET);
    println!("{}Total Energy Used: {:.2} units{}", GREEN, total_energy, RESET);
    println!("{}Path Length: {} steps{}", GREEN, path.len(), RESET);
    println!("{}Success Rate: {}{}", BRIGHT_GREEN, if path.is_empty() { "0%" } else { "100%" }, RESET);
    
    if !path.is_empty() {
        println!("\n{}{}{}", GREEN, "=== Flight Path Visualization ===", RESET);
        terrain.visualize_path(path);
        println!("\n{}{}{}", GREEN, "=== Legend ===", RESET);
        println!("{}S = Start Point{}", GREEN, RESET);
        println!("{}D = Destination{}", GREEN, RESET);
        println!("{}O = Obstacle{}", GREEN, RESET);
        println!("{}^ = Hill (High Cost){}", GREEN, RESET);
        println!("{}W = Wind Zone{}", GREEN, RESET);
        println!("{}* = Flight Path{}", GREEN, RESET);
        println!("{}.= Normal Terrain{}", GREEN, RESET);
    } else {
        println!("{}No path found! Target may be unreachable.{}", GREEN, RESET);
    }
}

fn save_performance_log(
    path: &Vec<Point>,
    total_time: f64,
    total_energy: f64,
    filename: &str,
) {
    let timestamp = std::time::SystemTime::now()
        .duration_since(std::time::UNIX_EPOCH)
        .unwrap()
        .as_secs();
    let success = if path.is_empty() { "false" } else { "true" };
    
    let csv_content = format!(
        "timestamp,algorithm,path_length,computation_time,energy_used,success\n{},A*,{},{},{},{}\n",
        timestamp, path.len(), total_time, total_energy, success
    );
    
    if let Ok(()) = fs::write(filename, csv_content) {
        println!("{}Performance log saved to: {}{}", GREEN, filename, RESET);
    } else {
        println!("{}Warning: Could not save performance log.{}", GREEN, RESET);
    }
}

fn main() {
    println!("{}{}{}", BRIGHT_GREEN, "=== UAV Flight Path Optimizer (AI-based) ===", RESET);
    println!("{}Simulating intelligent drone navigation with obstacle avoidance{}", GREEN, RESET);
    println!();
    
    let args: Vec<String> = env::args().collect();
    
    // Default parameters
    let mut map_file = "maps/sample_map.txt".to_string();
    let mut start = Point::new(0, 0);
    let mut end = Point::new(9, 9);
    
    // Parse command line arguments
    if args.len() == 6 {
        map_file = args[1].clone();
        start.x = args[2].parse().unwrap_or(0);
        start.y = args[3].parse().unwrap_or(0);
        end.x = args[4].parse().unwrap_or(9);
        end.y = args[5].parse().unwrap_or(9);
    } else if args.len() != 1 {
        print_usage();
        std::process::exit(1);
    }
    
    // Create output directory
    let _ = fs::create_dir_all("output");
    
    match run_simulation(&map_file, start, end) {
        Ok(()) => {
            println!("\n{}Simulation completed successfully!{}", BRIGHT_GREEN, RESET);
        }
        Err(e) => {
            eprintln!("{}Error: {}{}", GREEN, e, RESET);
            std::process::exit(1);
        }
    }
}

fn run_simulation(map_file: &str, start: Point, end: Point) -> Result<(), String> {
    // Initialize components
    let parser = MapParser::new();
    let terrain = parser.load_map(map_file)?;
    
    // Validate start and end points
    if !terrain.is_valid_position(&start) || !terrain.is_valid_position(&end) {
        return Err("Start or end position is invalid or blocked!".to_string());
    }
    
    if terrain.is_obstacle(&start) || terrain.is_obstacle(&end) {
        return Err("Start or end position is an obstacle!".to_string());
    }
    
    // Initialize drone and optimizer
    let drone = Drone::new(start, 1000.0);
    let optimizer = Optimizer::new(&terrain);
    
    println!("{}Map loaded: {}{}", GREEN, map_file, RESET);
    println!("{}Grid size: {}x{}{}", GREEN, terrain.get_width(), terrain.get_height(), RESET);
    println!("{}Start: ({}, {}){}", GREEN, start.x, start.y, RESET);
    println!("{}End: ({}, {}){}", GREEN, end.x, end.y, RESET);
    println!("{}Computing optimal path...{}", BRIGHT_GREEN, RESET);
    println!();
    
    // Measure computation time
    let start_time = Instant::now();
    
    // Find optimal path using A* algorithm
    let path = optimizer.find_path(&start, &end);
    
    let elapsed = start_time.elapsed();
    let total_time = elapsed.as_secs_f64();
    
    // Calculate total energy consumption
    let total_energy: f64 = path.iter()
        .map(|point| terrain.get_movement_cost(point))
        .sum();
    
    // Display results
    display_results(&path, &terrain, &drone, total_time, total_energy);
    
    // Save performance log
    save_performance_log(&path, total_time, total_energy, "output/path_log.csv");
    
    // Performance validation
    if total_time > 2.0 {
        println!("\n{}Warning: Computation time exceeded 2 seconds threshold!{}", GREEN, RESET);
    }
    
    if total_energy > drone.get_max_energy() * 0.85 {
        println!("\n{}Warning: Energy consumption exceeded 85% threshold!{}", GREEN, RESET);
    }
    
    Ok(())
}