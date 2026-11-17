#pragma once
#include "raylib.h"
#include "core/Maze.hpp"
#include "core/MazeSolver.hpp"
#include "render/MazeRenderer.hpp"
#include "render/Character.hpp"
#include "render/CameraController.hpp"
#include <string>

class UIManager {
private:
    float speedSliderValue = 25.0f;
    int currentMapIndex = 1;
    std::string currentMapLabel = "Mapa 1";

    // Variables de control de búsqueda
    AlgorithmType selectedAlgorithm = AlgorithmType::NONE;
    bool isSearchPlaying = false;
    float searchStepAccumulator = 0.0f;
    int currentSearchStep = 0;
    float searchSpeed = 25.0f; // Pasos por segundo

    void applyGirlyGuiTheme();

public:
    UIManager();

    bool loadMap(const std::string& mapPath, const std::string& label,
                    Maze& maze, MazeSolver& solver, MazeRenderer& renderer, Character& character,
                    CameraController& cameraController, float& offsetX, float& offsetZ, float cellSize);

    // Actualiza la reproducción de pasos de búsqueda
    void update(float dt, const MazeSolver& solver);

    void draw(Maze& maze, MazeSolver& solver, MazeRenderer& renderer, Character& character,
                CameraController& cameraController, float& offsetX, float& offsetZ, float cellSize, Font font);

    int getCurrentMap() const { return currentMapIndex; }
    int getCurrentSearchStep() const { return currentSearchStep; }
    AlgorithmType getSelectedAlgorithm() const { return selectedAlgorithm; }
};