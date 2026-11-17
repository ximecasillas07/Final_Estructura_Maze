#pragma once
#include "raylib.h"
#include "core/Maze.hpp"
#include "core/MazeSolver.hpp"

// Paleta de colores personalizable (Estilo Girly / Pastel)
struct GirlyTheme {
    Color background      = Color{ 22, 18, 30, 255 };    // Lavanda noche oscuro profundo
    Color wallNorthSouth  = Color{ 240, 110, 150, 255 }; // Rosa vibrante pastel
    Color wallEastWest    = Color{ 190, 120, 215, 255 }; // Lila orquídea
    Color wallWire        = Color{ 110, 40, 70, 255 };   // Contorno vino oscuro definido
    Color roof            = Color{ 255, 145, 180, 255 }; // Rosa pastel fresa
    Color roofBorder      = Color{ 140, 45, 80, 255 };   // Borde definido
    Color floor           = Color{ 245, 238, 230, 255 }; // Marfil suave transitable
    Color floorGrid       = Color{ 100, 80, 120, 80 };   // Cuadrícula sutil
    Color startA          = Color{ 40, 215, 175, 255 };  // Menta neón (Inicio)
    Color endB            = Color{ 255, 50, 110, 255 };  // Fresa neón brillante (Meta)

    // Colores para la búsqueda y solución
    Color searchStep      = Color{ 255, 195, 80, 210 };  // Ámbar / Dorado de exploración
    Color searchFrontier  = Color{ 80, 220, 240, 200 };  // Cian para frontera en BFS
    Color solutionPath    = Color{ 60, 240, 170, 240 };  // Esmeralda neón brillante para el camino final
};

class MazeRenderer {
private:
    float cellSize = 1.0f;
    float wallHeight = 1.0f;
    float wallThickness = 0.08f;

    // Banderas visuales modulares
    bool showRoofs = true;
    float wallAlpha = 1.0f;

    // Control de animación de construcción
    float revealProgress = 0.0f;
    float animationSpeed = 20.0f;
    bool animationComplete = false;

    // Tema activo
    GirlyTheme theme;

    // Métodos auxiliares de dibujo
    void drawModularWall(const Maze& maze, int r, int c, float posX, float posZ, float currentHeight);
    void drawFloorTile(float posX, float posZ);
    void drawMarkers(const Maze& maze, int r, int c, float posX, float posZ);

public:
    MazeRenderer() = default;

    void completeAnimation(int totalCells) {
        revealProgress = static_cast<float>(totalCells);
        animationComplete = true;
    }

    // Configuración
    void setCellSize(float size) { cellSize = size; }
    void setWallHeight(float height) { wallHeight = height; }
    void setShowRoofs(bool show) { showRoofs = show; }
    void toggleRoofs() { showRoofs = !showRoofs; }
    void setWallAlpha(float alpha) { wallAlpha = alpha; }
    void setAnimationSpeed(float speed) { animationSpeed = speed; }
    void resetAnimation() { revealProgress = 0.0f; animationComplete = false; }

    bool isAnimationFinished() const { return animationComplete; }
    float getAnimationSpeed() const { return animationSpeed; }
    bool getShowRoofs() const { return showRoofs; }

    // Actualización y renderizado
    void update(float deltaTime, int totalCells);
    void draw(const Maze& maze);

    // Renderizado de búsqueda y camino solución
    void drawSearchProgress(const Maze& maze, const MazeSolver& solver, int currentStep);
    void drawSolutionPath(const Maze& maze, const MazeSolver& solver);
};