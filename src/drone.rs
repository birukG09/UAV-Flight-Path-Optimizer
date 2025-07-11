use crate::terrain::Point;

pub struct Drone {
    position: Point,
    max_energy: f64,
    current_energy: f64,
    flight_path: Vec<Point>,
}

impl Drone {
    pub fn new(start_pos: Point, max_energy_capacity: f64) -> Self {
        let mut drone = Drone {
            position: start_pos,
            max_energy: max_energy_capacity,
            current_energy: max_energy_capacity,
            flight_path: Vec::new(),
        };
        drone.add_to_path(start_pos);
        drone
    }

    pub fn set_position(&mut self, pos: Point) {
        self.position = pos;
    }

    pub fn get_position(&self) -> Point {
        self.position
    }

    pub fn get_max_energy(&self) -> f64 {
        self.max_energy
    }

    pub fn get_current_energy(&self) -> f64 {
        self.current_energy
    }

    pub fn consume_energy(&mut self, amount: f64) {
        self.current_energy = (self.current_energy - amount).max(0.0);
    }

    pub fn reset_energy(&mut self) {
        self.current_energy = self.max_energy;
    }

    pub fn has_energy(&self, required: f64) -> bool {
        self.current_energy >= required
    }

    pub fn add_to_path(&mut self, point: Point) {
        self.flight_path.push(point);
    }

    pub fn get_flight_path(&self) -> &Vec<Point> {
        &self.flight_path
    }

    pub fn clear_path(&mut self) {
        self.flight_path.clear();
    }

    pub fn get_energy_percentage(&self) -> f64 {
        (self.current_energy / self.max_energy) * 100.0
    }

    pub fn is_low_energy(&self) -> bool {
        self.get_energy_percentage() < 20.0
    }

    pub fn display_status(&self) {
        println!("=== Drone Status ===");
        println!("Position: ({}, {})", self.position.x, self.position.y);
        println!("Energy: {:.1}/{:.1} ({:.1}%)", 
                 self.current_energy, self.max_energy, self.get_energy_percentage());
        println!("Path Length: {} waypoints", self.flight_path.len());
        
        if self.is_low_energy() {
            println!("WARNING: Low energy level!");
        }
        
        println!("===================");
    }
}