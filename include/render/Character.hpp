#pragma once
#include "raylib.h"
#include "core/Maze.hpp"
#include <vector>
#include <utility>

// Estructura para la estela de luz (Idea 4)
struct TrailNode {
    Vector3 position;
    float alpha;
};

// Estructura para el confeti 3D al llegar a la meta (Idea 2)
struct ConfettiParticle {
    Vector3 position;
    Vector3 velocity;
    Color color;
    float size;
};

class Character {
private:
    Vector3 currentPos = { 0.0f, 0.0f, 0.0f };
    Vector3 targetPos  = { 0.0f, 0.0f, 0.0f };

    int currentRow = 0;
    int currentCol = 0;
    bool isMoving = false;

    float yaw = 0.0f;
    float targetYaw = 0.0f;
    float moveSpeed = 4.5f;

    // Navegación automática por ruta solución
    std::vector<std::pair<int, int>> navigationPath;
    size_t currentPathIndex = 0;
    bool isAutoNavigating = false;
    bool reachedGoal = false;
    float celebrationTimer = 0.0f;

    // Estela de luz neón
    std::vector<TrailNode> trail;
    float trailTimer = 0.0f;

    // Partículas de confeti
    std::vector<ConfettiParticle> confetti;
    void spawnConfetti();

public:
    Character() = default;

    void init(const Maze& maze, float cellSize, float offsetX, float offsetZ);
    void update(float deltaTime, const Maze& maze, float cellSize, float offsetX, float offsetZ);
    void draw(bool visible);

    void startPathNavigation(const std::vector<std::pair<int, int>>& path, float cellSize, float offsetX, float offsetZ);
    void stopPathNavigation();

    Vector3 getPosition() const { return currentPos; }
    float getYaw() const { return yaw; }
    int getRow() const { return currentRow; }
    int getCol() const { return currentCol; }
    bool isAutoWalking() const { return isAutoNavigating; }
    bool hasReachedGoal() const { return reachedGoal; }
};