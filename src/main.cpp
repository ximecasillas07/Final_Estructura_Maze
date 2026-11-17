#include "raylib.h"
#include "core/Maze.hpp"
#include "core/MazeSolver.hpp"
#include "render/MazeRenderer.hpp"
#include "render/Character.hpp"
#include "render/CameraController.hpp"
#include "ui/UIManager.hpp"
#include <iostream>

int main() {
    Maze maze;
    if (!maze.loadFromFile("assets/maps/laberinto_1.in")) {
        std::cerr << "Error al cargar laberinto_1.in\n";
        return 1;
    }

    const int screenWidth = 1280;
    const int screenHeight = 720;

    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Laberinto 3D - Búsqueda y Solución");

    // Tipografía Segoe UI
    Font uiFont = GetFontDefault();
    if (FileExists("C:/Windows/Fonts/segoeui.ttf")) {
        uiFont = LoadFontEx("C:/Windows/Fonts/segoeui.ttf", 36, 0, 0);
        SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR);
    }

    const float cellSize = 1.0f;
    float offsetX = -(maze.getCols() * cellSize) / 2.0f + (cellSize / 2.0f);
    float offsetZ = -(maze.getRows() * cellSize) / 2.0f + (cellSize / 2.0f);

    MazeRenderer renderer;
    MazeSolver solver;
    Character character;
    CameraController cameraController;
    UIManager uiManager;

    character.init(maze, cellSize, offsetX, offsetZ);

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Soporte de Arrastrar y Soltar archivos .in a la ventana
        if (IsFileDropped()) {
            FilePathList droppedFiles = LoadDroppedFiles();
            if (droppedFiles.count > 0) {
                std::string path = droppedFiles.paths[0];
                std::string filenameOnly = path.substr(path.find_last_of("/\\") + 1);
                uiManager.loadMap(path, filenameOnly, maze, solver, renderer, character, cameraController, offsetX, offsetZ, cellSize);
            }
            UnloadDroppedFiles(droppedFiles);
        }

        // Atajos de teclado para las cámaras
        if (IsKeyPressed(KEY_ONE)) {
            cameraController.setMode(CameraState::ORBITAL);
            renderer.setWallAlpha(1.0f);
        }
        if (IsKeyPressed(KEY_TWO))   cameraController.setMode(CameraState::THIRD_PERSON);
        if (IsKeyPressed(KEY_THREE)) cameraController.setMode(CameraState::FIRST_PERSON);

        // Actualizar lógica del monito, cámara, animaciones y búsqueda
        character.update(dt, maze, cellSize, offsetX, offsetZ);
        cameraController.update(dt, character.getPosition(), character.getYaw());
        renderer.update(dt, maze.getRows() * maze.getCols());
        uiManager.update(dt, solver);

        BeginDrawing();
            ClearBackground(Color{ 22, 18, 30, 255 });

            // 1. Renderizado 3D de la escena
            BeginMode3D(cameraController.getCamera());
                renderer.draw(maze);

                // Dibujar progreso del algoritmo de búsqueda
                renderer.drawSearchProgress(maze, solver, uiManager.getCurrentSearchStep());

                // Si se encontró solución y terminó o avanzó la búsqueda, dibujar ruta
                if (solver.hasSolution() && uiManager.getCurrentSearchStep() >= solver.getTotalSteps()) {
                    renderer.drawSolutionPath(maze, solver);
                }

                character.draw(cameraController.shouldRenderCharacter());
            EndMode3D();

            // 2. Interfaz Gráfica interactiva con raygui (Paneles Izquierdo y Derecho)
            uiManager.draw(maze, solver, renderer, character, cameraController, offsetX, offsetZ, cellSize, uiFont);

            DrawFPS(GetScreenWidth() - 95, 20);
        EndDrawing();
    }

    if (uiFont.texture.id != GetFontDefault().texture.id) {
        UnloadFont(uiFont);
    }

    CloseWindow();
    return 0;
}