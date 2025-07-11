package com.uav.optimizer;

import javax.swing.*;
import javax.swing.border.EmptyBorder;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.util.*;
import java.util.List;

/**
 * UAV Flight Path Optimizer GUI Application
 * Provides a graphical interface for visualizing and controlling UAV flight path optimization
 */
public class UAVOptimizerGUI extends JFrame {
    
    // GUI Components
    private JPanel gridPanel;
    private JComboBox<String> algorithmSelector;
    private JButton startButton, resetButton, loadMapButton, saveButton;
    private JLabel statusLabel, statsLabel;
    private JSlider stepSlider;
    private JTextField gridSizeField, startXField, startYField, endXField, endYField;
    private JProgressBar progressBar;
    
    // Data structures
    private TerrainGrid terrain;
    private List<Point> flightPath;
    private Point startPoint, endPoint;
    private int currentStep = 0;
    private boolean isAnimating = false;
    
    // Constants
    private static final int CELL_SIZE = 20;
    private static final Color NORMAL_COLOR = new Color(240, 240, 240);
    private static final Color OBSTACLE_COLOR = new Color(100, 100, 100);
    private static final Color HILL_COLOR = new Color(139, 69, 19);
    private static final Color WIND_COLOR = new Color(173, 216, 230);
    private static final Color PATH_COLOR = new Color(0, 255, 0);
    private static final Color START_COLOR = new Color(0, 128, 255);
    private static final Color END_COLOR = new Color(255, 0, 0);
    private static final Color CURRENT_POS_COLOR = new Color(255, 255, 0);
    
    public UAVOptimizerGUI() {
        initializeComponents();
        setupLayout();
        loadDefaultMap();
    }
    
    private void initializeComponents() {
        setTitle("UAV Flight Path Optimizer - AI-based Navigation");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setResizable(true);
        
        // Control panel components
        algorithmSelector = new JComboBox<>(new String[]{"A*", "Dijkstra", "Greedy", "EnergyOptimal"});
        startButton = new JButton("Start Simulation");
        resetButton = new JButton("Reset");
        loadMapButton = new JButton("Load Map");
        saveButton = new JButton("Save Results");
        
        statusLabel = new JLabel("Ready to simulate UAV navigation");
        statsLabel = new JLabel("No simulation data available");
        
        stepSlider = new JSlider(0, 100, 0);
        stepSlider.setEnabled(false);
        
        gridSizeField = new JTextField("11", 3);
        startXField = new JTextField("0", 3);
        startYField = new JTextField("0", 3);
        endXField = new JTextField("9", 3);
        endYField = new JTextField("9", 3);
        
        progressBar = new JProgressBar(0, 100);
        progressBar.setVisible(false);
        
        // Initialize terrain with default size
        terrain = new TerrainGrid(11, 11);
        flightPath = new ArrayList<>();
        startPoint = new Point(0, 0);
        endPoint = new Point(9, 9);
        
        // Add event listeners
        setupEventListeners();
    }
    
    private void setupEventListeners() {
        startButton.addActionListener(e -> startSimulation());
        resetButton.addActionListener(e -> resetSimulation());
        loadMapButton.addActionListener(e -> loadMap());
        saveButton.addActionListener(e -> saveResults());
        
        stepSlider.addChangeListener(e -> {
            if (!isAnimating && flightPath != null && !flightPath.isEmpty()) {
                currentStep = stepSlider.getValue();
                repaintGrid();
            }
        });
        
        // Grid size change listener
        gridSizeField.addActionListener(e -> updateGridSize());
    }
    
    private void setupLayout() {
        setLayout(new BorderLayout());
        
        // Create main panels
        JPanel controlPanel = createControlPanel();
        JPanel visualizationPanel = createVisualizationPanel();
        JPanel statsPanel = createStatsPanel();
        
        // Add panels to frame
        add(controlPanel, BorderLayout.NORTH);
        add(visualizationPanel, BorderLayout.CENTER);
        add(statsPanel, BorderLayout.SOUTH);
        
        pack();
        setLocationRelativeTo(null);
    }
    
    private JPanel createControlPanel() {
        JPanel panel = new JPanel(new FlowLayout(FlowLayout.LEFT));
        panel.setBorder(new EmptyBorder(10, 10, 10, 10));
        
        // Algorithm selection
        panel.add(new JLabel("Algorithm:"));
        panel.add(algorithmSelector);
        panel.add(Box.createHorizontalStrut(20));
        
        // Grid configuration
        panel.add(new JLabel("Grid Size:"));
        panel.add(gridSizeField);
        panel.add(Box.createHorizontalStrut(10));
        
        // Start/End coordinates
        panel.add(new JLabel("Start ("));
        panel.add(startXField);
        panel.add(new JLabel(","));
        panel.add(startYField);
        panel.add(new JLabel(")"));
        panel.add(Box.createHorizontalStrut(10));
        
        panel.add(new JLabel("End ("));
        panel.add(endXField);
        panel.add(new JLabel(","));
        panel.add(endYField);
        panel.add(new JLabel(")"));
        panel.add(Box.createHorizontalStrut(20));
        
        // Action buttons
        panel.add(startButton);
        panel.add(resetButton);
        panel.add(loadMapButton);
        panel.add(saveButton);
        
        return panel;
    }
    
    private JPanel createVisualizationPanel() {
        JPanel panel = new JPanel(new BorderLayout());
        panel.setBorder(BorderFactory.createTitledBorder("Flight Path Visualization"));
        
        // Create grid panel
        gridPanel = new JPanel() {
            @Override
            protected void paintComponent(Graphics g) {
                super.paintComponent(g);
                drawGrid(g);
            }
        };
        
        updateGridPanelSize();
        
        // Add scroll pane for large grids
        JScrollPane scrollPane = new JScrollPane(gridPanel);
        scrollPane.setPreferredSize(new Dimension(600, 400));
        panel.add(scrollPane, BorderLayout.CENTER);
        
        // Add playback controls
        JPanel playbackPanel = new JPanel(new FlowLayout());
        playbackPanel.add(new JLabel("Path Step:"));
        playbackPanel.add(stepSlider);
        panel.add(playbackPanel, BorderLayout.SOUTH);
        
        return panel;
    }
    
    private JPanel createStatsPanel() {
        JPanel panel = new JPanel(new BorderLayout());
        panel.setBorder(BorderFactory.createTitledBorder("Simulation Statistics"));
        
        JPanel statusPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
        statusPanel.add(statusLabel);
        statusPanel.add(Box.createHorizontalStrut(20));
        statusPanel.add(progressBar);
        
        panel.add(statusPanel, BorderLayout.NORTH);
        panel.add(statsLabel, BorderLayout.CENTER);
        
        return panel;
    }
    
    private void updateGridPanelSize() {
        int width = terrain.getWidth() * CELL_SIZE;
        int height = terrain.getHeight() * CELL_SIZE;
        gridPanel.setPreferredSize(new Dimension(width, height));
        gridPanel.revalidate();
    }
    
    private void drawGrid(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        
        // Draw terrain
        for (int x = 0; x < terrain.getWidth(); x++) {
            for (int y = 0; y < terrain.getHeight(); y++) {
                Color cellColor = getTerrainColor(terrain.getCell(x, y));
                g2d.setColor(cellColor);
                g2d.fillRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
                
                // Draw grid lines
                g2d.setColor(Color.GRAY);
                g2d.drawRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
            }
        }
        
        // Draw flight path
        if (flightPath != null && !flightPath.isEmpty()) {
            g2d.setStroke(new BasicStroke(3));
            g2d.setColor(PATH_COLOR);
            
            for (int i = 0; i < Math.min(currentStep + 1, flightPath.size() - 1); i++) {
                Point p1 = flightPath.get(i);
                Point p2 = flightPath.get(i + 1);
                
                int x1 = p1.x * CELL_SIZE + CELL_SIZE / 2;
                int y1 = p1.y * CELL_SIZE + CELL_SIZE / 2;
                int x2 = p2.x * CELL_SIZE + CELL_SIZE / 2;
                int y2 = p2.y * CELL_SIZE + CELL_SIZE / 2;
                
                g2d.drawLine(x1, y1, x2, y2);
            }
        }
        
        // Draw start and end points
        drawSpecialPoint(g2d, startPoint, START_COLOR, "S");
        drawSpecialPoint(g2d, endPoint, END_COLOR, "E");
        
        // Draw current position if animating
        if (flightPath != null && !flightPath.isEmpty() && currentStep < flightPath.size()) {
            Point currentPos = flightPath.get(currentStep);
            drawSpecialPoint(g2d, currentPos, CURRENT_POS_COLOR, "UAV");
        }
    }
    
    private void drawSpecialPoint(Graphics2D g2d, Point point, Color color, String label) {
        int x = point.x * CELL_SIZE;
        int y = point.y * CELL_SIZE;
        
        g2d.setColor(color);
        g2d.fillOval(x + 2, y + 2, CELL_SIZE - 4, CELL_SIZE - 4);
        
        g2d.setColor(Color.BLACK);
        g2d.setFont(new Font("Arial", Font.BOLD, 10));
        FontMetrics fm = g2d.getFontMetrics();
        int textWidth = fm.stringWidth(label);
        int textHeight = fm.getHeight();
        g2d.drawString(label, x + (CELL_SIZE - textWidth) / 2, y + (CELL_SIZE + textHeight) / 2 - 2);
    }
    
    private Color getTerrainColor(TerrainType type) {
        switch (type) {
            case OBSTACLE: return OBSTACLE_COLOR;
            case HILL: return HILL_COLOR;
            case WIND: return WIND_COLOR;
            default: return NORMAL_COLOR;
        }
    }
    
    private void startSimulation() {
        // Validate inputs
        if (!validateInputs()) {
            return;
        }
        
        // Update coordinates
        startPoint = new Point(Integer.parseInt(startXField.getText()), 
                              Integer.parseInt(startYField.getText()));
        endPoint = new Point(Integer.parseInt(endXField.getText()), 
                            Integer.parseInt(endYField.getText()));
        
        // Disable controls during simulation
        setControlsEnabled(false);
        progressBar.setVisible(true);
        statusLabel.setText("Running simulation...");
        
        // Run simulation in background thread
        Thread simulationThread = new Thread(() -> {
            try {
                SimulationResult result = runBackendSimulation();
                SwingUtilities.invokeLater(() -> {
                    processSimulationResult(result);
                    setControlsEnabled(true);
                    progressBar.setVisible(false);
                });
            } catch (Exception e) {
                SwingUtilities.invokeLater(() -> {
                    JOptionPane.showMessageDialog(UAVOptimizerGUI.this, 
                        "Simulation failed: " + e.getMessage(), 
                        "Error", JOptionPane.ERROR_MESSAGE);
                    statusLabel.setText("Simulation failed");
                    setControlsEnabled(true);
                    progressBar.setVisible(false);
                });
            }
        });
        
        simulationThread.start();
    }
    
    private boolean validateInputs() {
        try {
            int gridSize = Integer.parseInt(gridSizeField.getText());
            int startX = Integer.parseInt(startXField.getText());
            int startY = Integer.parseInt(startYField.getText());
            int endX = Integer.parseInt(endXField.getText());
            int endY = Integer.parseInt(endYField.getText());
            
            if (gridSize < 5 || gridSize > 50) {
                JOptionPane.showMessageDialog(this, "Grid size must be between 5 and 50", 
                    "Invalid Input", JOptionPane.ERROR_MESSAGE);
                return false;
            }
            
            if (startX < 0 || startX >= gridSize || startY < 0 || startY >= gridSize ||
                endX < 0 || endX >= gridSize || endY < 0 || endY >= gridSize) {
                JOptionPane.showMessageDialog(this, "Start and end coordinates must be within grid bounds", 
                    "Invalid Input", JOptionPane.ERROR_MESSAGE);
                return false;
            }
            
            return true;
        } catch (NumberFormatException e) {
            JOptionPane.showMessageDialog(this, "Please enter valid numbers", 
                "Invalid Input", JOptionPane.ERROR_MESSAGE);
            return false;
        }
    }
    
    private SimulationResult runBackendSimulation() throws Exception {
        // Create temporary config file
        createConfigFile();
        
        // Run the C++ backend
        ProcessBuilder pb = new ProcessBuilder("./uav_optimizer");
        pb.directory(new File("."));
        Process process = pb.start();
        
        // Wait for completion
        int exitCode = process.waitFor();
        if (exitCode != 0) {
            throw new RuntimeException("Backend simulation failed with exit code: " + exitCode);
        }
        
        // Read results
        return readSimulationResults();
    }
    
    private void createConfigFile() throws IOException {
        // Create a simple config for the backend
        try (PrintWriter writer = new PrintWriter(new FileWriter("temp_config.json"))) {
            writer.println("{");
            writer.println("  \"algorithm\": \"" + algorithmSelector.getSelectedItem() + "\",");
            writer.println("  \"grid_size\": " + gridSizeField.getText() + ",");
            writer.println("  \"start_x\": " + startXField.getText() + ",");
            writer.println("  \"start_y\": " + startYField.getText() + ",");
            writer.println("  \"end_x\": " + endXField.getText() + ",");
            writer.println("  \"end_y\": " + endYField.getText());
            writer.println("}");
        }
    }
    
    private SimulationResult readSimulationResults() throws IOException {
        SimulationResult result = new SimulationResult();
        
        // Read CSV output
        File csvFile = new File("output/path_log.csv");
        if (csvFile.exists()) {
            try (BufferedReader reader = new BufferedReader(new FileReader(csvFile))) {
                String line;
                String lastLine = null;
                while ((line = reader.readLine()) != null) {
                    lastLine = line;
                }
                
                if (lastLine != null && !lastLine.startsWith("timestamp")) {
                    String[] parts = lastLine.split(",");
                    if (parts.length >= 6) {
                        result.algorithm = parts[1];
                        result.pathLength = Integer.parseInt(parts[2]);
                        result.computationTime = Double.parseDouble(parts[3]);
                        result.energyUsed = Double.parseDouble(parts[4]);
                        result.success = Boolean.parseBoolean(parts[5]);
                    }
                }
            }
        }
        
        // For now, simulate path reading - in a real implementation,
        // we would modify the backend to output the actual path coordinates
        result.path = generateSimulatedPath();
        
        return result;
    }
    
    private List<Point> generateSimulatedPath() {
        // Generate a simple diagonal path for demonstration
        List<Point> path = new ArrayList<>();
        int steps = Math.max(Math.abs(endPoint.x - startPoint.x), Math.abs(endPoint.y - startPoint.y));
        
        for (int i = 0; i <= steps; i++) {
            int x = startPoint.x + (int) Math.round((double) i * (endPoint.x - startPoint.x) / steps);
            int y = startPoint.y + (int) Math.round((double) i * (endPoint.y - startPoint.y) / steps);
            path.add(new Point(x, y));
        }
        
        return path;
    }
    
    private void processSimulationResult(SimulationResult result) {
        if (result.success) {
            flightPath = result.path;
            currentStep = 0;
            
            // Update slider
            stepSlider.setMaximum(flightPath.size() - 1);
            stepSlider.setValue(0);
            stepSlider.setEnabled(true);
            
            // Update statistics
            updateStatistics(result);
            statusLabel.setText("Simulation completed successfully!");
            
            // Start animation
            startPathAnimation();
        } else {
            statusLabel.setText("Simulation failed to find a valid path");
        }
        
        repaintGrid();
    }
    
    private void updateStatistics(SimulationResult result) {
        String stats = String.format(
            "<html><b>Algorithm:</b> %s | <b>Path Length:</b> %d steps | " +
            "<b>Energy Used:</b> %.2f units | <b>Computation Time:</b> %.3f seconds</html>",
            result.algorithm, result.pathLength, result.energyUsed, result.computationTime
        );
        statsLabel.setText(stats);
    }
    
    private void startPathAnimation() {
        if (flightPath == null || flightPath.isEmpty()) return;
        
        isAnimating = true;
        javax.swing.Timer timer = new javax.swing.Timer(200, new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (currentStep < flightPath.size() - 1) {
                    currentStep++;
                    stepSlider.setValue(currentStep);
                    repaintGrid();
                } else {
                    isAnimating = false;
                    ((javax.swing.Timer) e.getSource()).stop();
                }
            }
        });
        timer.start();
    }
    
    private void resetSimulation() {
        flightPath = null;
        currentStep = 0;
        stepSlider.setValue(0);
        stepSlider.setEnabled(false);
        statusLabel.setText("Ready to simulate UAV navigation");
        statsLabel.setText("No simulation data available");
        repaintGrid();
    }
    
    private void loadMap() {
        JFileChooser chooser = new JFileChooser("maps");
        chooser.setFileFilter(new javax.swing.filechooser.FileNameExtensionFilter("Text files", "txt"));
        
        if (chooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            try {
                loadMapFromFile(chooser.getSelectedFile());
                statusLabel.setText("Map loaded: " + chooser.getSelectedFile().getName());
            } catch (IOException e) {
                JOptionPane.showMessageDialog(this, "Failed to load map: " + e.getMessage(), 
                    "Error", JOptionPane.ERROR_MESSAGE);
            }
        }
    }
    
    private void loadMapFromFile(File file) throws IOException {
        List<String> lines = new ArrayList<>();
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = reader.readLine()) != null) {
                lines.add(line);
            }
        }
        
        if (lines.isEmpty()) {
            throw new IOException("Empty map file");
        }
        
        int height = lines.size();
        int width = lines.get(0).length();
        
        terrain = new TerrainGrid(width, height);
        
        for (int y = 0; y < height; y++) {
            String line = lines.get(y);
            for (int x = 0; x < Math.min(width, line.length()); x++) {
                char c = line.charAt(x);
                TerrainType type = getTerrainTypeFromChar(c);
                terrain.setCell(x, y, type);
            }
        }
        
        gridSizeField.setText(String.valueOf(width));
        updateGridPanelSize();
        repaintGrid();
    }
    
    private TerrainType getTerrainTypeFromChar(char c) {
        switch (c) {
            case 'O': return TerrainType.OBSTACLE;
            case '^': return TerrainType.HILL;
            case 'W': return TerrainType.WIND;
            default: return TerrainType.NORMAL;
        }
    }
    
    private void saveResults() {
        if (flightPath == null || flightPath.isEmpty()) {
            JOptionPane.showMessageDialog(this, "No simulation results to save", 
                "Information", JOptionPane.INFORMATION_MESSAGE);
            return;
        }
        
        JFileChooser chooser = new JFileChooser("output");
        chooser.setFileFilter(new javax.swing.filechooser.FileNameExtensionFilter("CSV files", "csv"));
        
        if (chooser.showSaveDialog(this) == JFileChooser.APPROVE_OPTION) {
            try {
                saveResultsToFile(chooser.getSelectedFile());
                statusLabel.setText("Results saved to: " + chooser.getSelectedFile().getName());
            } catch (IOException e) {
                JOptionPane.showMessageDialog(this, "Failed to save results: " + e.getMessage(), 
                    "Error", JOptionPane.ERROR_MESSAGE);
            }
        }
    }
    
    private void saveResultsToFile(File file) throws IOException {
        try (PrintWriter writer = new PrintWriter(new FileWriter(file))) {
            writer.println("step,x,y,terrain_type");
            for (int i = 0; i < flightPath.size(); i++) {
                Point p = flightPath.get(i);
                TerrainType type = terrain.getCell(p.x, p.y);
                writer.println(i + "," + p.x + "," + p.y + "," + type);
            }
        }
    }
    
    private void updateGridSize() {
        try {
            int newSize = Integer.parseInt(gridSizeField.getText());
            if (newSize >= 5 && newSize <= 50) {
                terrain = new TerrainGrid(newSize, newSize);
                loadDefaultMap();
                updateGridPanelSize();
                repaintGrid();
            }
        } catch (NumberFormatException e) {
            // Ignore invalid input
        }
    }
    
    private void loadDefaultMap() {
        // Load the default sample map if it exists
        File defaultMap = new File("maps/sample_map.txt");
        if (defaultMap.exists()) {
            try {
                loadMapFromFile(defaultMap);
            } catch (IOException e) {
                // Fall back to empty terrain
                createEmptyTerrain();
            }
        } else {
            createEmptyTerrain();
        }
    }
    
    private void createEmptyTerrain() {
        // Create a simple terrain with some obstacles for demonstration
        int size = terrain.getWidth();
        for (int x = 0; x < size; x++) {
            for (int y = 0; y < size; y++) {
                terrain.setCell(x, y, TerrainType.NORMAL);
            }
        }
        
        // Add some obstacles
        if (size >= 11) {
            terrain.setCell(2, 1, TerrainType.OBSTACLE);
            terrain.setCell(4, 1, TerrainType.OBSTACLE);
            terrain.setCell(6, 1, TerrainType.OBSTACLE);
            terrain.setCell(1, 3, TerrainType.OBSTACLE);
            terrain.setCell(7, 3, TerrainType.OBSTACLE);
            terrain.setCell(4, 3, TerrainType.HILL);
            terrain.setCell(3, 5, TerrainType.WIND);
            terrain.setCell(5, 5, TerrainType.WIND);
            terrain.setCell(7, 5, TerrainType.WIND);
        }
    }
    
    private void setControlsEnabled(boolean enabled) {
        startButton.setEnabled(enabled);
        loadMapButton.setEnabled(enabled);
        algorithmSelector.setEnabled(enabled);
        gridSizeField.setEnabled(enabled);
        startXField.setEnabled(enabled);
        startYField.setEnabled(enabled);
        endXField.setEnabled(enabled);
        endYField.setEnabled(enabled);
    }
    
    private void repaintGrid() {
        if (gridPanel != null) {
            gridPanel.repaint();
        }
    }
    
    // Inner classes for data structures
    private static class SimulationResult {
        String algorithm;
        int pathLength;
        double computationTime;
        double energyUsed;
        boolean success;
        List<Point> path;
    }
    
    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            // Use default look and feel
            new UAVOptimizerGUI().setVisible(true);
        });
    }
}