#include "render/MazeRenderer.hpp"
#include <algorithm>

void MazeRenderer::update(float deltaTime, int totalCells) {
    if (!animationComplete) {
        revealProgress += animationSpeed * deltaTime;
        if (revealProgress >= static_cast<float>(totalCells)) {
            revealProgress = static_cast<float>(totalCells);
            animationComplete = true;
        }
    }
}

void MazeRenderer::drawModularWall(const Maze& maze, int r, int c, float posX, float posZ, float currentHeight) {
    float halfSize = cellSize / 2.0f;
    float halfHeight = currentHeight / 2.0f;

    Color colorNS = Fade(theme.wallNorthSouth, wallAlpha);
    Color colorEW = Fade(theme.wallEastWest, wallAlpha);
    Color colorRoof = theme.roof;
    Color wireColor = Fade(theme.wallWire, wallAlpha);

    // 1. Cuerpo sólido del muro
    Vector3 blockPos = { posX, halfHeight, posZ };
    DrawCube(blockPos, cellSize, currentHeight, cellSize, colorNS);
    DrawCubeWires(blockPos, cellSize, currentHeight, cellSize, wireColor);

    // 2. Detalle de caras laterales
    DrawCube(Vector3{ posX, halfHeight, posZ - halfSize + 0.01f }, cellSize, currentHeight, 0.02f, colorNS);
    DrawCube(Vector3{ posX, halfHeight, posZ + halfSize - 0.01f }, cellSize, currentHeight, 0.02f, colorNS);
    DrawCube(Vector3{ posX - halfSize + 0.01f, halfHeight, posZ }, 0.02f, currentHeight, cellSize, colorEW);
    DrawCube(Vector3{ posX + halfSize - 0.01f, halfHeight, posZ }, 0.02f, currentHeight, cellSize, colorEW);

    // 3. Tapa superior (+Y)
    if (showRoofs && currentHeight >= (wallHeight * 0.9f)) {
        Vector3 posRoof = { posX, currentHeight + 0.01f, posZ };
        DrawCube(posRoof, cellSize * 0.98f, 0.03f, cellSize * 0.98f, colorRoof);
        DrawCubeWires(posRoof, cellSize * 0.98f, 0.03f, cellSize * 0.98f, theme.roofBorder);
    }
}

void MazeRenderer::drawFloorTile(float posX, float posZ) {
    Vector3 floorPos = { posX, 0.01f, posZ };
    DrawCube(floorPos, cellSize * 0.98f, 0.02f, cellSize * 0.98f, theme.floor);
}

void MazeRenderer::drawMarkers(const Maze& maze, int r, int c, float posX, float posZ) {
    // Marcador Inicio 'A' (Menta pastel)
    if (r == maze.getStart().first && c == maze.getStart().second) {
        DrawCylinder(Vector3{ posX, 0.03f, posZ }, 0.42f, 0.42f, 0.12f, 24, theme.startA);
        DrawSphere(Vector3{ posX, 0.35f, posZ }, 0.22f, theme.startA);
    }
    // Marcador Meta 'B' (Fresa vibrante)
    if (r == maze.getEnd().first && c == maze.getEnd().second) {
        DrawCylinder(Vector3{ posX, 0.03f, posZ }, 0.42f, 0.42f, 0.12f, 24, theme.endB);
        DrawSphere(Vector3{ posX, 0.35f, posZ }, 0.25f, theme.endB);
    }
}

void MazeRenderer::draw(const Maze& maze) {
    int rows = maze.getRows();
    int cols = maze.getCols();

    float offsetX = -(cols * cellSize) / 2.0f + (cellSize / 2.0f);
    float offsetZ = -(rows * cellSize) / 2.0f + (cellSize / 2.0f);

    int cellIndex = 0;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            float posX = (c * cellSize) + offsetX;
            float posZ = (r * cellSize) + offsetZ;

            // Suelo transitable
            if (!maze.isWall(r, c)) {
                drawFloorTile(posX, posZ);
            }

            // Marcadores de A y B
            drawMarkers(maze, r, c, posX, posZ);

            // Muros con animación de elevación
            if (maze.isWall(r, c)) {
                if (cellIndex <= static_cast<int>(revealProgress)) {
                    float progressFactor = revealProgress - static_cast<float>(cellIndex);
                    float heightFactor = std::min(1.0f, progressFactor);
                    float currentHeight = wallHeight * heightFactor;

                    drawModularWall(maze, r, c, posX, posZ, currentHeight);
                }
            }

            cellIndex++;
        }
    }
}

// Dibuja las casillas exploradas según el paso actual de la animación
void MazeRenderer::drawSearchProgress(const Maze& maze, const MazeSolver& solver, int currentStep) {
    const auto& steps = solver.getSteps();
    if (steps.empty() || currentStep <= 0) return;

    float offsetX = -(maze.getCols() * cellSize) / 2.0f + (cellSize / 2.0f);
    float offsetZ = -(maze.getRows() * cellSize) / 2.0f + (cellSize / 2.0f);

    int limit = std::min(currentStep, static_cast<int>(steps.size()));
    for (int i = 0; i < limit; ++i) {
        int r = steps[i].coord.first;
        int c = steps[i].coord.second;
        float posX = (c * cellSize) + offsetX;
        float posZ = (r * cellSize) + offsetZ;

        // Baldosa translúcida sobre el suelo
        Color stepColor = (steps[i].type == StepType::FRONTIER) ? theme.searchFrontier : theme.searchStep;
        DrawCube(Vector3{ posX, 0.025f, posZ }, cellSize * 0.92f, 0.03f, cellSize * 0.92f, stepColor);

        // Si es el paso actual activo (la cabeza de exploración), dibujamos un halo brillante
        if (i == limit - 1) {
            DrawCircle3D(Vector3{ posX, 0.05f, posZ }, 0.35f, Vector3{ 1.0f, 0.0f, 0.0f }, 90.0f, Color{ 255, 255, 255, 220 });
        }
    }
}

// Dibuja el camino solución final iluminado
void MazeRenderer::drawSolutionPath(const Maze& maze, const MazeSolver& solver) {
    const auto& path = solver.getSolutionPath();
    if (path.empty()) return;

    float offsetX = -(maze.getCols() * cellSize) / 2.0f + (cellSize / 2.0f);
    float offsetZ = -(maze.getRows() * cellSize) / 2.0f + (cellSize / 2.0f);

    for (size_t i = 0; i < path.size(); ++i) {
        int r = path[i].first;
        int c = path[i].second;
        float posX = (c * cellSize) + offsetX;
        float posZ = (r * cellSize) + offsetZ;

        // Baldosa esmeralda luminosa para la ruta
        DrawCube(Vector3{ posX, 0.035f, posZ }, cellSize * 0.70f, 0.04f, cellSize * 0.70f, theme.solutionPath);
        DrawCubeWires(Vector3{ posX, 0.035f, posZ }, cellSize * 0.70f, 0.04f, cellSize * 0.70f, Color{ 255, 255, 255, 180 });
    }
}