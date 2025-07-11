# ✈️ UAV Flight Path Optimizer

A multi-language simulation and optimization system for unmanned aerial vehicle (UAV) path planning. This project combines the strengths of **C++**, **Java**, and **Rust** to build a highly modular, high-performance flight simulation console application with ASCII UI inspired by Metasploit.

---

## 🧠 Overview

**UAV Flight Path Optimizer** is a console-based tool designed to simulate terrain navigation, AI-based flight behavior, and pathfinding for UAVs and autonomous aircraft. It includes:

- Real-time flight simulations
- Metasploit-style ASCII graphics
- Interactive terrain and pathfinding system
- AI-based decision modeling (Java)
- Performance-critical flight logic (Rust)
![image alt](https://github.com/birukG09/UAV-Flight-Path-Optimizer/blob/1a5fe8a4726a51372cf93f0487de8521a5ee6187/Screenshot%202025-07-11%20191246.png)
![image alt](https://github.com/birukG09/UAV-Flight-Path-Optimizer/blob/846faf3e2e1c932c352ff56752918145aaf67ae0/Screenshot%202025-07-11%20191304.png)

## 📦 Features

| Feature                          | Description |
|----------------------------------|-------------|
| 🗺️ Terrain Map Display           | Shows obstacle maps with symbols |
| 🎯 Set Coordinates                | Define start and end points for flight |
| 🧭 Pathfinding Algorithms         | Supports A*, Dijkstra, BFS, DFS |
| 🎮 Step-by-step Simulation        | Navigate with mission playback |
| ⚙️ Debug Mode                     | Toggle real-time analysis and logs |
| 📊 Stats View                    | Show energy use, cost, time, distance |
| 📁 Export Logs                    | Save simulations as JSON/LOG files |
| 🛡️ Safety Logic (Rust)           | Verifies safe flight constraints |
| 🧬 Real-Time Entity Deployment    | Deploy drone entities mid-simulation |
| 🌍 Map Generator/Loader          | Load terrain or auto-generate |

---

## 🗂️ Folder Structure

FlightPathPlanner/
├── include/ # Header files
├── src/ # C++ source code
├── java/ # Java UAV behavior
├── attached_assets/ # ASCII UI art
├── output/ # Exported logs
├── maps/ # Custom map data
├── obj/ # Object files
├── config.json # Global config
├── uav_optimizer.exe # Main Windows binary
├── Makefile # Linux build
├── rust_build.sh # Rust build script
└── run_gui.sh # Optional GUI launcher (Linux)

yaml
Copy
Edit

---

## 💻 How to Run

### 🔹 On Windows

```bash
g++ -o uav_optimizer.exe main.cpp src/*.cpp
.\uav_optimizer.exe
🔹 On Replit/Linux
bash
Copy
Edit
chmod +x uav_optimizer
./uav_optimizer
🔹 Compile Java Modules
bash
Copy
Edit
javac java/uav_demo.java
java uav_demo
🔹 Run Rust Logic
bash
Copy
Edit
sh rust_build.sh
🧮 Console Menu Example
css
Copy
Edit
╔════════════════════════════════════════════════╗
║             ✈ UAV Control Panel ✈             ║
╠════════════════════════════════════════════════╣
║  1. [SCAN] Display terrain map                ║
║  2. [COORD] Set navigation coordinates        ║
║  3. [ALGO] Select pathfinding algorithm       ║
║  4. [EXEC] Execute simulation                 ║
║  5. [PLAY] Step-by-step mission playback      ║
║  6. [DEBUG] Toggle debug/analysis mode        ║
║  7. [STATS] Display mission statistics        ║
║  8. [EXPORT] Export mission data              ║
║  9. [AI] AI performance analytics             ║
║ 10. [DEPLOY] Deploy entities                  ║
║ 11. [REAL] Toggle real-time mode              ║
║ 12. [GEN] Generate new terrain                ║
║ 13. [HELP] System documentation               ║
║ 14. [EXIT] Shutdown system                    ║
╚════════════════════════════════════════════════╝
