use crate::terrain::{Terrain, Point};
use std::collections::{HashMap, BinaryHeap};
use std::cmp::Ordering;

#[derive(Debug, Clone)]
struct PathNode {
    position: Point,
    g_cost: f64,  // Distance from start
    h_cost: f64,  // Heuristic distance to goal
    f_cost: f64,  // Total cost (g + h)
    parent: Option<Point>,
}

impl PathNode {
    fn new(position: Point, g_cost: f64, h_cost: f64, parent: Option<Point>) -> Self {
        PathNode {
            position,
            g_cost,
            h_cost,
            f_cost: g_cost + h_cost,
            parent,
        }
    }
}

impl Eq for PathNode {}

impl PartialEq for PathNode {
    fn eq(&self, other: &Self) -> bool {
        self.position == other.position
    }
}

impl Ord for PathNode {
    fn cmp(&self, other: &Self) -> Ordering {
        // Reverse ordering for min-heap behavior
        other.f_cost.partial_cmp(&self.f_cost).unwrap_or(Ordering::Equal)
            .then_with(|| other.h_cost.partial_cmp(&self.h_cost).unwrap_or(Ordering::Equal))
    }
}

impl PartialOrd for PathNode {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

pub struct Optimizer<'a> {
    terrain: &'a Terrain,
}

impl<'a> Optimizer<'a> {
    pub fn new(terrain: &'a Terrain) -> Self {
        Optimizer { terrain }
    }

    pub fn find_path(&self, start: &Point, goal: &Point) -> Vec<Point> {
        self.find_path_astar(start, goal)
    }

    pub fn find_path_astar(&self, start: &Point, goal: &Point) -> Vec<Point> {
        let mut open_set = BinaryHeap::new();
        let mut all_nodes: HashMap<Point, PathNode> = HashMap::new();
        let mut g_score: HashMap<Point, f64> = HashMap::new();

        // Initialize start node
        let start_node = PathNode::new(*start, 0.0, self.terrain.get_heuristic_cost(start, goal), None);
        open_set.push(start_node.clone());
        all_nodes.insert(*start, start_node);
        g_score.insert(*start, 0.0);

        while let Some(current) = open_set.pop() {
            // Check if we reached the goal
            if current.position == *goal {
                return self.reconstruct_path(&all_nodes, &current.position);
            }

            // Explore neighbors
            for neighbor in self.terrain.get_neighbors(&current.position) {
                let movement_cost = self.terrain.get_movement_cost(&neighbor);
                
                // Skip if obstacle
                if movement_cost >= 1000.0 {
                    continue;
                }

                // Calculate diagonal movement cost
                let distance = self.calculate_distance(&current.position, &neighbor);
                let tentative_g_score = current.g_cost + movement_cost * distance;

                // Check if this path to neighbor is better
                if !g_score.contains_key(&neighbor) || tentative_g_score < g_score[&neighbor] {
                    g_score.insert(neighbor, tentative_g_score);
                    
                    let h_cost = self.terrain.get_heuristic_cost(&neighbor, goal);
                    let neighbor_node = PathNode::new(neighbor, tentative_g_score, h_cost, Some(current.position));
                    
                    all_nodes.insert(neighbor, neighbor_node.clone());
                    open_set.push(neighbor_node);
                }
            }
        }

        // No path found
        Vec::new()
    }

    pub fn find_path_dijkstra(&self, start: &Point, goal: &Point) -> Vec<Point> {
        let mut open_set = BinaryHeap::new();
        let mut all_nodes: HashMap<Point, PathNode> = HashMap::new();
        let mut distances: HashMap<Point, f64> = HashMap::new();

        // Initialize start node (no heuristic for Dijkstra)
        let start_node = PathNode::new(*start, 0.0, 0.0, None);
        open_set.push(start_node.clone());
        all_nodes.insert(*start, start_node);
        distances.insert(*start, 0.0);

        while let Some(current) = open_set.pop() {
            if current.position == *goal {
                return self.reconstruct_path(&all_nodes, &current.position);
            }

            for neighbor in self.terrain.get_neighbors(&current.position) {
                let movement_cost = self.terrain.get_movement_cost(&neighbor);
                if movement_cost >= 1000.0 {
                    continue;
                }

                let distance = self.calculate_distance(&current.position, &neighbor);
                let new_distance = current.g_cost + movement_cost * distance;

                if !distances.contains_key(&neighbor) || new_distance < distances[&neighbor] {
                    distances.insert(neighbor, new_distance);
                    
                    let neighbor_node = PathNode::new(neighbor, new_distance, 0.0, Some(current.position));
                    all_nodes.insert(neighbor, neighbor_node.clone());
                    open_set.push(neighbor_node);
                }
            }
        }

        Vec::new()
    }

    pub fn find_path_greedy(&self, start: &Point, goal: &Point) -> Vec<Point> {
        let mut path = Vec::new();
        let mut current = *start;
        path.push(current);

        while current != *goal {
            let neighbors = self.terrain.get_neighbors(&current);
            
            if neighbors.is_empty() {
                break;
            }

            // Find neighbor with minimum heuristic cost to goal
            let mut best_neighbor = neighbors[0];
            let mut best_cost = self.terrain.get_heuristic_cost(&best_neighbor, goal);

            for neighbor in neighbors {
                let cost = self.terrain.get_heuristic_cost(&neighbor, goal);
                if cost < best_cost {
                    best_cost = cost;
                    best_neighbor = neighbor;
                }
            }

            current = best_neighbor;
            path.push(current);

            // Prevent infinite loops
            if path.len() > self.terrain.get_width() * self.terrain.get_height() {
                break;
            }
        }

        path
    }

    pub fn find_energy_optimal_path(&self, start: &Point, goal: &Point, energy_weight: f64) -> Vec<Point> {
        let mut open_set = BinaryHeap::new();
        let mut all_nodes: HashMap<Point, PathNode> = HashMap::new();
        let mut g_score: HashMap<Point, f64> = HashMap::new();

        let start_node = PathNode::new(*start, 0.0, self.terrain.get_heuristic_cost(start, goal), None);
        open_set.push(start_node.clone());
        all_nodes.insert(*start, start_node);
        g_score.insert(*start, 0.0);

        while let Some(current) = open_set.pop() {
            if current.position == *goal {
                return self.reconstruct_path(&all_nodes, &current.position);
            }

            for neighbor in self.terrain.get_neighbors(&current.position) {
                let movement_cost = self.terrain.get_movement_cost(&neighbor);
                if movement_cost >= 1000.0 {
                    continue;
                }

                let distance = self.calculate_distance(&current.position, &neighbor);
                
                // Energy-weighted cost function
                let energy_cost = movement_cost * energy_weight;
                let tentative_g_score = current.g_cost + energy_cost * distance;

                if !g_score.contains_key(&neighbor) || tentative_g_score < g_score[&neighbor] {
                    g_score.insert(neighbor, tentative_g_score);
                    
                    let h_cost = self.terrain.get_heuristic_cost(&neighbor, goal);
                    let neighbor_node = PathNode::new(neighbor, tentative_g_score, h_cost, Some(current.position));
                    
                    all_nodes.insert(neighbor, neighbor_node.clone());
                    open_set.push(neighbor_node);
                }
            }
        }

        Vec::new()
    }

    fn reconstruct_path(&self, all_nodes: &HashMap<Point, PathNode>, goal: &Point) -> Vec<Point> {
        let mut path = Vec::new();
        let mut current = *goal;
        
        while let Some(node) = all_nodes.get(&current) {
            path.push(current);
            if let Some(parent) = node.parent {
                current = parent;
            } else {
                break;
            }
        }
        
        path.reverse();
        path
    }

    fn calculate_distance(&self, a: &Point, b: &Point) -> f64 {
        let dx = (a.x - b.x) as f64;
        let dy = (a.y - b.y) as f64;
        (dx * dx + dy * dy).sqrt()
    }

    pub fn calculate_path_cost(&self, path: &Vec<Point>) -> f64 {
        if path.is_empty() {
            return 0.0;
        }
        
        path.iter().map(|point| self.terrain.get_movement_cost(point)).sum()
    }

    pub fn is_path_valid(&self, path: &Vec<Point>) -> bool {
        path.iter().all(|point| self.terrain.is_passable(point))
    }

    pub fn print_path_statistics(&self, path: &Vec<Point>) {
        if path.is_empty() {
            println!("No path found!");
            return;
        }

        let total_cost = self.calculate_path_cost(path);
        let mut total_distance = 0.0;

        for i in 1..path.len() {
            total_distance += self.calculate_distance(&path[i-1], &path[i]);
        }

        println!("=== Path Statistics ===");
        println!("Path Length: {} steps", path.len());
        println!("Total Distance: {:.2} units", total_distance);
        println!("Total Cost: {:.2} energy units", total_cost);
        println!("Average Cost per Step: {:.2}", total_cost / path.len() as f64);
        println!("Path Valid: {}", if self.is_path_valid(path) { "Yes" } else { "No" });
        println!("======================");
    }
}