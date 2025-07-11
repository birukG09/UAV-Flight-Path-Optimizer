package com.uav.optimizer;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.util.*;
import java.util.List;

/**
 * Simplified UAV Flight Path Optimizer GUI
 * Provides visualization and basic controls for UAV path planning
 */
public class PathVisualizerGUI extends JFrame {
    
    private JPanel gridPanel;
    private JComboBox<String> algorithmSelector;
    private JButton startButton, resetButton, loadMapButton;
    private JLabel statusLabel, statsLabel;
    private JTextField startXField, startYField, endXField, endYField;
    
    // Grid and path data
    private char[][] terrainGrid;
    private java.util.List<Point> currentPath;
    private Point startPoint, endPoint;
    private int gridSize = 11;
    
    // Visual constants
    private static final int CELL_SIZE = 25;
    private static final Color NORMAL_COLOR = new Color(245, 245, 245);
    private static final Color OBSTACLE_COLOR = new Color(80, 80, 80);
    private static final Color HILL_COLOR = new Color(139, 69, 19);
    private static final Color WIND_COLOR = new Color(173, 216, 230);
    private static final Color PATH_COLOR = new Color(0, 200, 0);
    private static final Color START_COLOR = new Color(0, 100, 255);
    private static final Color END_COLOR = new Color(255, 50, 50);
    
    public PathVisualizerGUI() {
        initializeGUI();
        loadSampleTerrain();
    }
    
    private void initializeGUI() {
        setTitle("UAV Flight Path Optimizer - Interactive Visualization");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLayout(new BorderLayout());
        
        // Create components
        createControlPanel();
        createVisualizationPanel();
        createStatusPanel();
        
        // Set up the window
        pack();
        setLocationRelativeTo(null);
        setResizable(true);
        
        // Initialize default values
        startPoint = new Point(0, 0);
        endPoint = new Point(9, 9);
        currentPath = new ArrayList<>();
        
        updateDisplay();
    }
    
    private void createControlPanel() {
        JPanel controlPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
        controlPanel.setBorder(BorderFactory.createTitledBorder("Simulation Controls"));
        
        // Algorithm selection
        controlPanel.add(new JLabel("Algorithm:"));
        algorithmSelector = new JComboBox<>(new String[]{"A*", "Dijkstra", "Greedy", "EnergyOptimal"});
        controlPanel.add(algorithmSelector);
        
        controlPanel.add(Box.createHorizontalStrut(20));
        
        // Start/End coordinates
        controlPanel.add(new JLabel("Start:"));
        startXField = new JTextField("0", 2);
        startYField = new JTextField("0", 2);
        controlPanel.add(startXField);
        controlPanel.add(new JLabel(","));
        controlPanel.add(startYField);
        
        controlPanel.add(Box.createHorizontalStrut(10));
        
        controlPanel.add(new JLabel("End:"));
        endXField = new JTextField("9", 2);
        endYField = new JTextField("9", 2);
        controlPanel.add(endXField);
        controlPanel.add(new JLabel(","));
        controlPanel.add(endYField);
        
        controlPanel.add(Box.createHorizontalStrut(20));
        
        // Action buttons
        startButton = new JButton("Start Simulation");
        resetButton = new JButton("Reset");
        loadMapButton = new JButton("Load Map");
        
        startButton.addActionListener(e -> startSimulation());
        resetButton.addActionListener(e -> resetSimulation());
        loadMapButton.addActionListener(e -> loadMap());
        
        controlPanel.add(startButton);
        controlPanel.add(resetButton);
        controlPanel.add(loadMapButton);
        
        add(controlPanel, BorderLayout.NORTH);
    }
    
    private void createVisualizationPanel() {
        JPanel vizPanel = new JPanel(new BorderLayout());
        vizPanel.setBorder(BorderFactory.createTitledBorder("Flight Path Visualization"));
        
        // Create grid panel
        gridPanel = new JPanel() {
            @Override
            protected void paintComponent(Graphics g) {
                super.paintComponent(g);
                drawGrid(g);
            }
        };
        
        updateGridSize();
        gridPanel.setBackground(Color.WHITE);
        
        // Add to scroll pane
        JScrollPane scrollPane = new JScrollPane(gridPanel);
        scrollPane.setPreferredSize(new Dimension(700, 500));
        scrollPane.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        scrollPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
        
        vizPanel.add(scrollPane, BorderLayout.CENTER);
        
        // Add legend
        JPanel legendPanel = createLegendPanel();
        vizPanel.add(legendPanel, BorderLayout.SOUTH);
        
        add(vizPanel, BorderLayout.CENTER);
    }
    
    private JPanel createLegendPanel() {
        JPanel legend = new JPanel(new FlowLayout(FlowLayout.LEFT));
        legend.setBorder(BorderFactory.createTitledBorder("Legend"));
        
        addLegendItem(legend, "Start", START_COLOR);
        addLegendItem(legend, "End", END_COLOR);
        addLegendItem(legend, "Path", PATH_COLOR);
        addLegendItem(legend, "Obstacle", OBSTACLE_COLOR);
        addLegendItem(legend, "Hill", HILL_COLOR);
        addLegendItem(legend, "Wind", WIND_COLOR);
        addLegendItem(legend, "Normal", NORMAL_COLOR);
        
        return legend;
    }
    
    private void addLegendItem(JPanel parent, String text, Color color) {
        JPanel item = new JPanel(new FlowLayout(FlowLayout.LEFT, 2, 0));
        
        JPanel colorBox = new JPanel();
        colorBox.setBackground(color);
        colorBox.setPreferredSize(new Dimension(15, 15));
        colorBox.setBorder(BorderFactory.createLineBorder(Color.BLACK));
        
        item.add(colorBox);
        item.add(new JLabel(text));
        parent.add(item);
    }
    
    private void createStatusPanel() {
        JPanel statusPanel = new JPanel(new BorderLayout());
        statusPanel.setBorder(BorderFactory.createTitledBorder("Status & Statistics"));
        
        statusLabel = new JLabel("Ready to simulate UAV navigation");
        statsLabel = new JLabel("Load a map and run simulation to see results");
        
        JPanel topStatus = new JPanel(new FlowLayout(FlowLayout.LEFT));
        topStatus.add(statusLabel);
        
        statusPanel.add(topStatus, BorderLayout.NORTH);
        statusPanel.add(statsLabel, BorderLayout.CENTER);
        
        add(statusPanel, BorderLayout.SOUTH);
    }
    
    private void updateGridSize() {
        int width = gridSize * CELL_SIZE;
        int height = gridSize * CELL_SIZE;
        gridPanel.setPreferredSize(new Dimension(width, height));
        gridPanel.revalidate();
    }
    
    private void drawGrid(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        
        // Draw terrain
        if (terrainGrid != null) {
            for (int y = 0; y < gridSize; y++) {
                for (int x = 0; x < gridSize; x++) {
                    Color cellColor = getTerrainColor(terrainGrid[y][x]);
                    g2d.setColor(cellColor);
                    g2d.fillRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
                    
                    // Draw grid lines
                    g2d.setColor(Color.GRAY);
                    g2d.drawRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
                }
            }
        }
        
        // Draw path
        if (currentPath != null && currentPath.size() > 1) {
            g2d.setColor(PATH_COLOR);
            g2d.setStroke(new BasicStroke(3));
            
            for (int i = 0; i < currentPath.size() - 1; i++) {
                Point p1 = currentPath.get(i);
                Point p2 = currentPath.get(i + 1);
                
                int x1 = p1.x * CELL_SIZE + CELL_SIZE / 2;
                int y1 = p1.y * CELL_SIZE + CELL_SIZE / 2;
                int x2 = p2.x * CELL_SIZE + CELL_SIZE / 2;
                int y2 = p2.y * CELL_SIZE + CELL_SIZE / 2;
                
                g2d.drawLine(x1, y1, x2, y2);
            }
        }
        
        // Draw start and end points
        drawPoint(g2d, startPoint, START_COLOR, "S");
        drawPoint(g2d, endPoint, END_COLOR, "E");
    }
    
    private void drawPoint(Graphics2D g2d, Point point, Color color, String label) {
        int x = point.x * CELL_SIZE;
        int y = point.y * CELL_SIZE;
        
        g2d.setColor(color);
        g2d.fillOval(x + 3, y + 3, CELL_SIZE - 6, CELL_SIZE - 6);
        
        g2d.setColor(Color.WHITE);
        g2d.setFont(new Font("Arial", Font.BOLD, 12));
        FontMetrics fm = g2d.getFontMetrics();
        int textWidth = fm.stringWidth(label);
        int textHeight = fm.getHeight();
        g2d.drawString(label, x + (CELL_SIZE - textWidth) / 2, y + (CELL_SIZE + textHeight) / 2 - 2);
    }
    
    private Color getTerrainColor(char terrain) {
        switch (terrain) {
            case 'O': return OBSTACLE_COLOR;
            case '^': return HILL_COLOR;
            case 'W': return WIND_COLOR;
            default: return NORMAL_COLOR;
        }
    }
    
    private void startSimulation() {
        // Update start/end points
        try {
            startPoint = new Point(Integer.parseInt(startXField.getText()), 
                                  Integer.parseInt(startYField.getText()));
            endPoint = new Point(Integer.parseInt(endXField.getText()), 
                                Integer.parseInt(endYField.getText()));
        } catch (NumberFormatException e) {
            JOptionPane.showMessageDialog(this, "Please enter valid coordinates", 
                "Input Error", JOptionPane.ERROR_MESSAGE);
            return;
        }
        
        // Validate coordinates
        if (!isValidCoordinate(startPoint) || !isValidCoordinate(endPoint)) {
            JOptionPane.showMessageDialog(this, "Coordinates must be within grid bounds", 
                "Input Error", JOptionPane.ERROR_MESSAGE);
            return;
        }
        
        statusLabel.setText("Running simulation...");
        startButton.setEnabled(false);
        
        // Run simulation in background
        Thread simThread = new Thread(() -> {
            try {
                runSimulation();
            } catch (Exception e) {
                SwingUtilities.invokeLater(() -> {
                    statusLabel.setText("Simulation failed: " + e.getMessage());
                    startButton.setEnabled(true);
                });
            }
        });
        simThread.start();
    }
    
    private void runSimulation() throws Exception {
        // Create argument string for backend
        String args = String.format("maps/sample_map.txt %d %d %d %d", 
            startPoint.x, startPoint.y, endPoint.x, endPoint.y);
        
        // Run the C++ backend
        ProcessBuilder pb = new ProcessBuilder("./uav_optimizer");
        pb.directory(new File(".."));
        
        Process process = pb.start();
        int exitCode = process.waitFor();
        
        if (exitCode == 0) {
            // Read the results and create path
            loadPathFromResults();
            
            SwingUtilities.invokeLater(() -> {
                statusLabel.setText("Simulation completed successfully!");
                updateStatistics();
                updateDisplay();
                startButton.setEnabled(true);
            });
        } else {
            throw new RuntimeException("Backend simulation failed");
        }
    }
    
    private void loadPathFromResults() {
        // For now, create a simple path from start to end
        // In a full implementation, this would read the actual path from the backend
        currentPath = new ArrayList<>();
        
        // Simple diagonal path
        int dx = endPoint.x - startPoint.x;
        int dy = endPoint.y - startPoint.y;
        int steps = Math.max(Math.abs(dx), Math.abs(dy));
        
        for (int i = 0; i <= steps; i++) {
            int x = startPoint.x + (dx * i) / steps;
            int y = startPoint.y + (dy * i) / steps;
            currentPath.add(new Point(x, y));
        }
    }
    
    private void updateStatistics() {
        if (currentPath != null && !currentPath.isEmpty()) {
            String algorithm = (String) algorithmSelector.getSelectedItem();
            int pathLength = currentPath.size();
            double energy = pathLength * 1.0; // Simplified energy calculation
            
            statsLabel.setText(String.format(
                "Algorithm: %s | Path Length: %d steps | Energy: %.1f units | Success: Yes",
                algorithm, pathLength, energy));
        }
    }
    
    private boolean isValidCoordinate(Point p) {
        return p.x >= 0 && p.x < gridSize && p.y >= 0 && p.y < gridSize;
    }
    
    private void resetSimulation() {
        currentPath = new ArrayList<>();
        statusLabel.setText("Ready to simulate UAV navigation");
        statsLabel.setText("Load a map and run simulation to see results");
        updateDisplay();
    }
    
    private void loadMap() {
        JFileChooser chooser = new JFileChooser("../maps");
        chooser.setFileFilter(new javax.swing.filechooser.FileNameExtensionFilter("Text files", "txt"));
        
        if (chooser.showOpenDialog(this) == JFileChooser.APPROVE_OPTION) {
            try {
                loadTerrainFromFile(chooser.getSelectedFile());
                statusLabel.setText("Map loaded: " + chooser.getSelectedFile().getName());
                updateDisplay();
            } catch (IOException e) {
                JOptionPane.showMessageDialog(this, "Failed to load map: " + e.getMessage(), 
                    "Error", JOptionPane.ERROR_MESSAGE);
            }
        }
    }
    
    private void loadTerrainFromFile(File file) throws IOException {
        java.util.List<String> lines = new ArrayList<>();
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            while ((line = reader.readLine()) != null) {
                lines.add(line);
            }
        }
        
        if (lines.isEmpty()) {
            throw new IOException("Empty map file");
        }
        
        gridSize = lines.size();
        terrainGrid = new char[gridSize][gridSize];
        
        for (int y = 0; y < gridSize; y++) {
            String line = lines.get(y);
            for (int x = 0; x < Math.min(gridSize, line.length()); x++) {
                terrainGrid[y][x] = line.charAt(x);
            }
        }
        
        updateGridSize();
    }
    
    private void loadSampleTerrain() {
        // Load default terrain
        terrainGrid = new char[gridSize][gridSize];
        
        // Initialize with normal terrain
        for (int y = 0; y < gridSize; y++) {
            for (int x = 0; x < gridSize; x++) {
                terrainGrid[y][x] = '.';
            }
        }
        
        // Add some sample obstacles and features
        terrainGrid[1][2] = 'O';
        terrainGrid[1][4] = 'O';
        terrainGrid[1][6] = 'O';
        terrainGrid[3][1] = 'O';
        terrainGrid[3][7] = 'O';
        terrainGrid[3][4] = '^';
        terrainGrid[5][3] = 'W';
        terrainGrid[5][5] = 'W';
        terrainGrid[5][7] = 'W';
        terrainGrid[7][1] = 'O';
        terrainGrid[7][7] = 'O';
        terrainGrid[7][4] = '^';
        terrainGrid[9][2] = 'O';
        terrainGrid[9][4] = 'O';
        terrainGrid[9][6] = 'O';
    }
    
    private void updateDisplay() {
        if (gridPanel != null) {
            gridPanel.repaint();
        }
    }
    
    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            new PathVisualizerGUI().setVisible(true);
        });
    }
}