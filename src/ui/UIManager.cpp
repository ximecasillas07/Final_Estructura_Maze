#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "ui/UIManager.hpp"
#include "ui/FileDialog.hpp"
#include <iostream>
#include <algorithm>

UIManager::UIManager() {
    applyGirlyGuiTheme();
}

void UIManager::applyGirlyGuiTheme() {
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 0x1A1426FF);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0xF5E5F5FF);
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 0xFFFFFFFF);
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, 0xFFAAD0FF);

    GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, 0xDC78A0FF);
    GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, 0x2E1E3EFF);
    GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, 0x4A2C60FF);
    GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, 0x7A356EFF);

    GuiSetStyle(SLIDER, BORDER_COLOR_NORMAL, 0xDC78A0FF);
    GuiSetStyle(SLIDER, BASE_COLOR_NORMAL, 0x2E1E3EFF);
    GuiSetStyle(SLIDER, BASE_COLOR_PRESSED, 0xFF85B2FF);
}

bool UIManager::loadMap(const std::string& mapPath, const std::string& label,
                        Maze& maze, MazeSolver& solver, MazeRenderer& renderer, Character& character,
                        CameraController& cameraController, float& offsetX, float& offsetZ, float cellSize) {
    if (maze.loadFromFile(mapPath)) {
        currentMapLabel = label;

        offsetX = -(maze.getCols() * cellSize) / 2.0f + (cellSize / 2.0f);
        offsetZ = -(maze.getRows() * cellSize) / 2.0f + (cellSize / 2.0f);

        cameraController.autoFit(maze.getRows(), maze.getCols(), cellSize);

        int total = maze.getRows() * maze.getCols();
        if (total > 500) {
            speedSliderValue = static_cast<float>(total) / 2.5f;
        } else {
            speedSliderValue = 30.0f;
        }
        renderer.setAnimationSpeed(speedSliderValue);

        character.init(maze, cellSize, offsetX, offsetZ);
        renderer.resetAnimation();

        // Reiniciar búsqueda y estadísticas al cargar nuevo mapa
        solver.reset();
        solver.resetStats();
        selectedAlgorithm = AlgorithmType::NONE;
        isSearchPlaying = false;
        currentSearchStep = 0;
        searchStepAccumulator = 0.0f;

        return true;
    }
    return false;
}

void UIManager::update(float dt, const MazeSolver& solver) {
    int totalSteps = solver.getTotalSteps();
    if (isSearchPlaying && totalSteps > 0) {
        searchStepAccumulator += searchSpeed * dt;
        while (searchStepAccumulator >= 1.0f && currentSearchStep < totalSteps) {
            currentSearchStep++;
            searchStepAccumulator -= 1.0f;
        }
        if (currentSearchStep >= totalSteps) {
            currentSearchStep = totalSteps;
            isSearchPlaying = false;
        }
    }
}

void UIManager::draw(Maze& maze, MazeSolver& solver, MazeRenderer& renderer, Character& character,
                    CameraController& cameraController, float& offsetX, float& offsetZ, float cellSize, Font font) {

    // ==========================================
    // PANEL IZQUIERDO: Control General y Escena
    // ==========================================
    Rectangle panelLeft = { 15, 15, 345, 430 };
    DrawRectangleRec(panelLeft, Fade(Color{ 18, 14, 26, 255 }, 0.92f));
    DrawRectangleLinesEx(panelLeft, 1.5f, Color{ 220, 120, 160, 255 });

    DrawTextEx(font, "PANEL DE CONTROL", Vector2{ 30, 25 }, 18, 1.0f, Color{ 255, 180, 215, 255 });

    // 1. Selector de Cámaras
    DrawTextEx(font, "Modo de Camara:", Vector2{ 30, 55 }, 14, 1.0f, LIGHTGRAY);

    bool isCenital = (cameraController.getMode() == CameraState::ORBITAL);
    bool isThird   = (cameraController.getMode() == CameraState::THIRD_PERSON);
    bool isFirst   = (cameraController.getMode() == CameraState::FIRST_PERSON);

    static bool isXrayActive = false;

    if (GuiButton(Rectangle{ 30, 75, 95, 28 }, isCenital ? "#120#Cenital" : "Cenital")) {
        cameraController.setMode(CameraState::ORBITAL);
        renderer.setWallAlpha(1.0f);
    }
    if (GuiButton(Rectangle{ 135, 75, 105, 28 }, isThird ? "#120#3ra Persona" : "3ra Persona")) {
        cameraController.setMode(CameraState::THIRD_PERSON);
        renderer.setWallAlpha(isXrayActive ? 0.35f : 1.0f);
    }
    if (GuiButton(Rectangle{ 250, 75, 95, 28 }, isFirst ? "#120#1ra Persona" : "1ra Persona")) {
        cameraController.setMode(CameraState::FIRST_PERSON);
        renderer.setWallAlpha(isXrayActive ? 0.35f : 1.0f);
    }

    // 2. Modos Visuales (Solo Rayos-X en 3ra y 1ra persona)
    if (!isCenital) {
        DrawTextEx(font, "Opciones Visuales:", Vector2{ 30, 115 }, 14, 1.0f, LIGHTGRAY);

        if (GuiButton(Rectangle{ 30, 135, 315, 28 }, isXrayActive ? "Rayos-X: ACTIVO" : "Rayos-X: NORMAL")) {
            isXrayActive = !isXrayActive;
            renderer.setWallAlpha(isXrayActive ? 0.35f : 1.0f);
        }
    } else {
        renderer.setWallAlpha(1.0f);
        DrawTextEx(font, "Opciones Visuales: No aplicable en Cenital", Vector2{ 30, 125 }, 13, 1.0f, DARKGRAY);
    }

    // 3. Velocidad y Botón Mostrar Todo
    DrawTextEx(font, TextFormat("Velocidad: %.0f celdas/seg", speedSliderValue), Vector2{ 30, 175 }, 14, 1.0f, Color{ 80, 240, 190, 255
    });
    GuiSliderBar(Rectangle{ 30, 195, 185, 20 }, "", "", &speedSliderValue, 5.0f, 200.0f);
    renderer.setAnimationSpeed(speedSliderValue);

    if (GuiButton(Rectangle{ 225, 195, 55, 20 }, "Reset")) {
        renderer.resetAnimation();
    }
    if (GuiButton(Rectangle{ 285, 195, 60, 20 }, "Ver Todo")) {
        renderer.completeAnimation(maze.getRows() * maze.getCols());
    }

    // 4. Selector de Laberintos
    DrawTextEx(font, "Cargar Laberinto:", Vector2{ 30, 230 }, 14, 1.0f, LIGHTGRAY);
    DrawTextEx(font, TextFormat("Actual: %s (%dx%d)", currentMapLabel.c_str(), maze.getRows(), maze.getCols()), Vector2{ 30, 250 }, 13,
    1.0f, Color{ 255, 215, 0, 255 });

    for (int i = 1; i <= 6; ++i) {
        int col = (i - 1) % 3;
        int row = (i - 1) / 3;
        Rectangle btnRect = { 30.0f + (col * 110.0f), 275.0f + (row * 35.0f), 100.0f, 28.0f };

        std::string mapLabel = "Mapa " + std::to_string(i);
        if (GuiButton(btnRect, mapLabel.c_str())) {
            std::string mapPath = "assets/maps/laberinto_" + std::to_string(i) + ".in";
            currentMapIndex = i;
            loadMap(mapPath, mapLabel, maze, solver, renderer, character, cameraController, offsetX, offsetZ, cellSize);
        }
    }

    if (GuiButton(Rectangle{ 30, 350, 315, 28 }, "#003# Cargar Archivo Propio (.in/.txt)")) {
        std::string selectedFile = OpenFileDialogWindows();
        if (!selectedFile.empty()) {
            std::string filenameOnly = selectedFile.substr(selectedFile.find_last_of("/\\") + 1);
            currentMapIndex = -1;
            loadMap(selectedFile, filenameOnly, maze, solver, renderer, character, cameraController, offsetX, offsetZ, cellSize);
        }
    }

    DrawTextEx(font, "W,A,S,D: caminar | O arrastra un .in a la ventana", Vector2{ 30, 395 }, 12, 1.0f, LIGHTGRAY);

    // ==========================================
    // PANEL DERECHO: Algoritmos y Comparativa
    // ==========================================
    float rightX = static_cast<float>(GetScreenWidth()) - 355.0f;
    Rectangle panelRight = { rightX, 15, 340, 430 }; // Altura 430 para incluir la comparativa
    DrawRectangleRec(panelRight, Fade(Color{ 18, 14, 26, 255 }, 0.92f));
    DrawRectangleLinesEx(panelRight, 1.5f, Color{ 140, 100, 220, 255 });

    DrawTextEx(font, "ALGORITMOS DE BUSQUEDA", Vector2{ rightX + 15, 25 }, 16, 1.0f, Color{ 200, 170, 255, 255 });

    bool isPre  = (selectedAlgorithm == AlgorithmType::PREORDER);
    bool isPost = (selectedAlgorithm == AlgorithmType::POSTORDER);
    bool isDFS  = (selectedAlgorithm == AlgorithmType::DFS);
    bool isBFS  = (selectedAlgorithm == AlgorithmType::BFS);

    auto startAlgorithm = [&](AlgorithmType alg) {
        selectedAlgorithm = alg;
        solver.solve(maze, alg);
        currentSearchStep = 0;
        searchStepAccumulator = 0.0f;
        isSearchPlaying = true;
        character.stopPathNavigation();
    };

    if (GuiButton(Rectangle{ rightX + 15, 55, 150, 28 }, isPre ? "#120#Preorden" : "Preorden")) {
        startAlgorithm(AlgorithmType::PREORDER);
    }
    if (GuiButton(Rectangle{ rightX + 175, 55, 150, 28 }, isPost ? "#120#Postorden" : "Postorden")) {
        startAlgorithm(AlgorithmType::POSTORDER);
    }
    if (GuiButton(Rectangle{ rightX + 15, 90, 150, 28 }, isDFS ? "#120#DFS" : "DFS")) {
        startAlgorithm(AlgorithmType::DFS);
    }
    if (GuiButton(Rectangle{ rightX + 175, 90, 150, 28 }, isBFS ? "#120#BFS" : "BFS")) {
        startAlgorithm(AlgorithmType::BFS);
    }

    // Controles de Reproducción
    DrawTextEx(font, "Control de Reproduccion:", Vector2{ rightX + 15, 130 }, 14, 1.0f, LIGHTGRAY);

    if (GuiButton(Rectangle{ rightX + 15, 150, 72, 28 }, isSearchPlaying ? "Pausa" : "Play")) {
        if (solver.getTotalSteps() > 0) {
            if (currentSearchStep >= solver.getTotalSteps()) currentSearchStep = 0;
            isSearchPlaying = !isSearchPlaying;
        }
    }
    if (GuiButton(Rectangle{ rightX + 92, 150, 72, 28 }, "Reset")) {
        isSearchPlaying = false;
        currentSearchStep = 0;
        searchStepAccumulator = 0.0f;
        character.stopPathNavigation();
    }
    if (GuiButton(Rectangle{ rightX + 169, 150, 72, 28 }, "Paso +1")) {
        isSearchPlaying = false;
        if (currentSearchStep < solver.getTotalSteps()) currentSearchStep++;
    }
    if (GuiButton(Rectangle{ rightX + 246, 150, 78, 28 }, "Fin")) {
        isSearchPlaying = false;
        currentSearchStep = solver.getTotalSteps();
    }

    // Slider de Velocidad de Búsqueda
    DrawTextEx(font, TextFormat("Velocidad: %.0f pasos/seg", searchSpeed), Vector2{ rightX + 15, 190 }, 14, 1.0f, Color{ 80, 240, 190,
    255 });
    GuiSliderBar(Rectangle{ rightX + 15, 210, 310, 20 }, "", "", &searchSpeed, 2.0f, 150.0f);

    // Estado e Información del Algoritmo
    int totalSteps = solver.getTotalSteps();
    DrawTextEx(font, TextFormat("Paso: %d / %d", currentSearchStep, totalSteps), Vector2{ rightX + 15, 242 }, 14, 1.0f, WHITE);

    if (totalSteps > 0 && currentSearchStep >= totalSteps) {
        if (solver.hasSolution()) {
            DrawTextEx(font, TextFormat("Meta Alcanzada! Ruta: %d pasos", (int)solver.getSolutionPath().size()),
                        Vector2{ rightX + 15, 265 }, 13, 1.0f, Color{ 60, 240, 170, 255 });

            if (character.isAutoWalking()) {
                if (GuiButton(Rectangle{ rightX + 15, 288, 310, 30 }, "#131# Detener Caminata")) {
                    character.stopPathNavigation();
                }
            } else {
                if (GuiButton(Rectangle{ rightX + 15, 288, 310, 30 }, "#131# Recorrer Solucion")) {
                    character.startPathNavigation(solver.getSolutionPath(), cellSize, offsetX, offsetZ);
                }
            }
        }
    } else if (isSearchPlaying) {
        DrawTextEx(font, "Explorando casillas...", Vector2{ rightX + 15, 265 }, 13, 1.0f, Color{ 255, 215, 0, 255 });
    }

    // ==========================================
    // CUADRO COMPARATIVO DE EFICIENCIA (Idea 3)
    // ==========================================
    Rectangle cmpBox = { rightX + 15, 328, 310, 90 };
    DrawRectangleRec(cmpBox, Fade(Color{ 10, 8, 16, 255 }, 0.85f));
    DrawRectangleLinesEx(cmpBox, 1.0f, Color{ 90, 70, 130, 255 });

    DrawTextEx(font, "COMPARATIVA DE EFICIENCIA:", Vector2{ rightX + 22, 335 }, 12, 1.0f, Color{ 255, 200, 100, 255 });

    auto dfsS = solver.getDFSStats();
    auto bfsS = solver.getBFSStats();

    if (dfsS.calculated) {
        DrawTextEx(font, TextFormat("DFS: %d explorados | Ruta: %d", dfsS.nodesExplored, dfsS.pathLength),
                    Vector2{ rightX + 22, 355 }, 12, 1.0f, LIGHTGRAY);
    } else {
        DrawTextEx(font, "DFS: (Presiona DFS para medir)", Vector2{ rightX + 22, 355 }, 12, 1.0f, DARKGRAY);
    }

    if (bfsS.calculated) {
        DrawTextEx(font, TextFormat("BFS: %d explorados | Ruta: %d (Optima)", bfsS.nodesExplored, bfsS.pathLength),
                    Vector2{ rightX + 22, 375 }, 12, 1.0f, Color{ 80, 240, 190, 255 });
    } else {
        DrawTextEx(font, "BFS: (Presiona BFS para medir)", Vector2{ rightX + 22, 375 }, 12, 1.0f, DARKGRAY);
    }

    DrawTextEx(font, "*BFS garantiza la ruta minima", Vector2{ rightX + 22, 398 }, 11, 1.0f, GRAY);
}