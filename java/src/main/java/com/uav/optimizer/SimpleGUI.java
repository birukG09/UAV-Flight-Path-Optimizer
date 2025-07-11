package com.uav.optimizer;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.*;
import java.util.*;
import java.util.List;

/**
 * Simple UAV Flight Path Optimizer GUI
 * Basic functional GUI for UAV path planning
 */
public class SimpleGUI extends JFrame {
    
    private JPanel mapPanel;
    private JButton startButton, resetButton;
    private JLabel statusLabel;
    private JComboBox<String> algorithmBox;
    private JTextField startXField, startYField, endXField, endYField;
    
    private char[][] terrain;
    private List<Point> path;
    private Point start, end;
    private int gridSize = 11;
    private static final int CELL_SIZE = 30;
    
    public SimpleGUI() {
        initGUI();
        loadDefaultTerrain();
        start = new Point(0, 0);
        end = new Point(9, 9);
        path = new ArrayList<>();
    }
    
    private void initGUI() {
        setTitle("UAV Flight Path Optimizer");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setLayout(new BorderLayout());
        
        // Control panel
        JPanel controlPanel = new JPanel(new FlowLayout());
        controlPanel.add(new JLabel("Algorithm:"));
        algorithmBox = new JComboBox<>(new String[]{"A*", "Dijkstra", "Greedy"});
        controlPanel.add(algorithmBox);
        
        controlPanel.add(new JLabel("Start:"));
        startXField = new JTextField("0", 2);
        startYField = new JTextField("0", 2);
        controlPanel.add(startXField);
        controlPanel.add(startYField);
        
        controlPanel.add(new JLabel("End:"));
        endXField = new JTextField("9", 2);
        endYField = new JTextField("9", 2);
        controlPanel.add(endXField);
        controlPanel.add(endYField);
        
        startButton = new JButton("Start Simulation");
        resetButton = new JButton("Reset");
        controlPanel.add(startButton);
        controlPanel.add(resetButton);
        
        add(controlPanel, BorderLayout.NORTH);
        
        // Map panel
        mapPanel = new JPanel() {
            @Override
            protected void paintComponent(Graphics g) {
                super.paintComponent(g);
                drawMap(g);
            }
        };
        mapPanel.setPreferredSize(new Dimension(gridSize * CELL_SIZE, gridSize * CELL_SIZE));
        mapPanel.setBackground(Color.WHITE);
        add(mapPanel, BorderLayout.CENTER);
        
        // Status panel
        statusLabel = new JLabel("Ready for simulation");
        add(statusLabel, BorderLayout.SOUTH);
        
        // Event listeners
        startButton.addActionListener(e -> runSimulation());
        resetButton.addActionListener(e -> resetPath());
        
        pack();
        setLocationRelativeTo(null);
    }
    
    private void drawMap(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;
        
        // Draw terrain
        for (int y = 0; y < gridSize; y++) {
            for (int x = 0; x < gridSize; x++) {
                Color color = getTerrainColor(terrain[y][x]);
                g2d.setColor(color);
                g2d.fillRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
                g2d.setColor(Color.BLACK);
                g2d.drawRect(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE);
            }
        }
        
        // Draw path
        if (path.size() > 1) {
            g2d.setColor(Color.GREEN);
            g2d.setStroke(new BasicStroke(3));
            for (int i = 0; i < path.size() - 1; i++) {
                Point p1 = path.get(i);
                Point p2 = path.get(i + 1);
                g2d.drawLine(p1.x * CELL_SIZE + CELL_SIZE/2, p1.y * CELL_SIZE + CELL_SIZE/2,
                           p2.x * CELL_SIZE + CELL_SIZE/2, p2.y * CELL_SIZE + CELL_SIZE/2);
            }
        }
        
        // Draw start and end
        g2d.setColor(Color.BLUE);
        g2d.fillOval(start.x * CELL_SIZE + 5, start.y * CELL_SIZE + 5, CELL_SIZE - 10, CELL_SIZE - 10);
        g2d.setColor(Color.RED);
        g2d.fillOval(end.x * CELL_SIZE + 5, end.y * CELL_SIZE + 5, CELL_SIZE - 10, CELL_SIZE - 10);
    }
    
    private Color getTerrainColor(char c) {
        switch (c) {
            case 'O': return Color.DARK_GRAY;
            case '^': return new Color(139, 69, 19);
            case 'W': return Color.CYAN;
            default: return Color.LIGHT_GRAY;
        }
    }
    
    private void runSimulation() {
        // Update start/end points
        try {
            start = new Point(Integer.parseInt(startXField.getText()), Integer.parseInt(startYField.getText()));
            end = new Point(Integer.parseInt(endXField.getText()), Integer.parseInt(endYField.getText()));
        } catch (NumberFormatException e) {
            statusLabel.setText("Invalid coordinates");
            return;
        }
        
        // Generate simple path
        path = generatePath(start, end);
        statusLabel.setText("Path generated: " + path.size() + " steps");
        mapPanel.repaint();
    }
    
    private List<Point> generatePath(Point start, Point end) {
        List<Point> result = new ArrayList<>();
        
        // Simple diagonal path
        int dx = end.x - start.x;
        int dy = end.y - start.y;
        int steps = Math.max(Math.abs(dx), Math.abs(dy));
        
        for (int i = 0; i <= steps; i++) {
            int x = start.x + (dx * i) / steps;
            int y = start.y + (dy * i) / steps;
            result.add(new Point(x, y));
        }
        
        return result;
    }
    
    private void resetPath() {
        path.clear();
        statusLabel.setText("Path reset");
        mapPanel.repaint();
    }
    
    private void loadDefaultTerrain() {
        terrain = new char[gridSize][gridSize];
        
        // Fill with normal terrain
        for (int y = 0; y < gridSize; y++) {
            for (int x = 0; x < gridSize; x++) {
                terrain[y][x] = '.';
            }
        }
        
        // Add obstacles
        terrain[1][2] = 'O';
        terrain[1][4] = 'O';
        terrain[1][6] = 'O';
        terrain[3][1] = 'O';
        terrain[3][7] = 'O';
        terrain[3][4] = '^';
        terrain[5][3] = 'W';
        terrain[5][5] = 'W';
        terrain[5][7] = 'W';
        terrain[7][1] = 'O';
        terrain[7][7] = 'O';
        terrain[7][4] = '^';
        terrain[9][2] = 'O';
        terrain[9][4] = 'O';
        terrain[9][6] = 'O';
    }
    
    public static void main(String[] args) {
        SwingUtilities.invokeLater(() -> {
            new SimpleGUI().setVisible(true);
        });
    }
}