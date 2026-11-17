#include "render/Character.hpp"
#include "raymath.h"
#include <cmath>

void Character::init(const Maze& maze, float cellSize, float offsetX, float offsetZ) {
    auto start = maze.getStart();
    currentRow = start.first;
    currentCol = start.second;

    currentPos = Vector3{
        (currentCol * cellSize) + offsetX,
        0.0f,
        (currentRow * cellSize) + offsetZ
    };
    targetPos = currentPos;
    yaw = 0.0f;
    targetYaw = 0.0f;
    isMoving = false;
    isAutoNavigating = false;
    reachedGoal = false;
    celebrationTimer = 0.0f;
    navigationPath.clear();
    currentPathIndex = 0;

    trail.clear();
    confetti.clear();
}

void Character::spawnConfetti() {
    confetti.clear();
    Color palette[] = {
        Color{ 255, 60, 120, 255 },  // Fucsia
        Color{ 255, 220, 50, 255 },  // Dorado
        Color{ 50, 230, 200, 255 },  // Turquesa
        Color{ 180, 80, 255, 255 },  // Violeta
        Color{ 255, 255, 255, 255 }  // Blanco destello
    };

    for (int i = 0; i < 90; ++i) {
        float vx = (float)GetRandomValue(-35, 35) / 10.0f;
        float vy = (float)GetRandomValue(35, 75) / 10.0f;
        float vz = (float)GetRandomValue(-35, 35) / 10.0f;

        ConfettiParticle p;
        p.position = Vector3{ currentPos.x, 0.4f, currentPos.z };
        p.velocity = Vector3{ vx, vy, vz };
        p.color = palette[GetRandomValue(0, 4)];
        p.size = (float)GetRandomValue(5, 12) / 100.0f;
        confetti.push_back(p);
    }
}

void Character::startPathNavigation(const std::vector<std::pair<int, int>>& path, float cellSize, float offsetX, float offsetZ) {
    if (path.empty()) return;

    navigationPath = path;
    currentPathIndex = 0;
    isAutoNavigating = true;
    reachedGoal = false;
    celebrationTimer = 0.0f;
    confetti.clear();

    currentRow = navigationPath[0].first;
    currentCol = navigationPath[0].second;
    currentPos = Vector3{
        (currentCol * cellSize) + offsetX,
        0.0f,
        (currentRow * cellSize) + offsetZ
    };
    targetPos = currentPos;
    isMoving = false;
}

void Character::stopPathNavigation() {
    isAutoNavigating = false;
}

void Character::update(float deltaTime, const Maze& maze, float cellSize, float offsetX, float offsetZ) {
    // ------------------------------------------
    // A. Actualizar Estela Neón (Idea 4)
    // ------------------------------------------
    if (isMoving) {
        trailTimer += deltaTime;
        if (trailTimer >= 0.05f) {
            trailTimer = 0.0f;
            trail.push_back({ currentPos, 1.0f });
        }
    }
    for (auto it = trail.begin(); it != trail.end();) {
        it->alpha -= 1.2f * deltaTime; // Desvanecimiento suave
        if (it->alpha <= 0.0f) {
            it = trail.erase(it);
        } else {
            ++it;
        }
    }

    // ------------------------------------------
    // B. Actualizar Confeti al Ganar (Idea 2)
    // ------------------------------------------
    if (reachedGoal) {
        celebrationTimer += deltaTime;
        for (auto& p : confetti) {
            p.position.x += p.velocity.x * deltaTime;
            p.position.y += p.velocity.y * deltaTime;
            p.position.z += p.velocity.z * deltaTime;

            p.velocity.y -= 7.5f * deltaTime; // Gravedad suave

            if (p.position.y < 0.02f) {
                p.position.y = 0.02f;
                p.velocity.y = -p.velocity.y * 0.25f; // Rebote suave en el piso
            }
        }
    }

    // ------------------------------------------
    // C. Movimiento Continuo
    // ------------------------------------------
    if (isMoving) {
        currentPos = Vector3MoveTowards(currentPos, targetPos, moveSpeed * deltaTime);

        float diff = targetYaw - yaw;
        while (diff < -180.0f) diff += 360.0f;
        while (diff > 180.0f) diff -= 360.0f;
        yaw += diff * 12.0f * deltaTime;

        if (Vector3Distance(currentPos, targetPos) < 0.01f) {
            currentPos = targetPos;
            yaw = targetYaw;
            isMoving = false;
        }
        return;
    }

    // ------------------------------------------
    // D. Navegación Automática
    // ------------------------------------------
    if (isAutoNavigating) {
        if (currentPathIndex + 1 < navigationPath.size()) {
            currentPathIndex++;
            int nextR = navigationPath[currentPathIndex].first;
            int nextC = navigationPath[currentPathIndex].second;

            int dr = nextR - currentRow;
            int dc = nextC - currentCol;
            if (dr == 1)  targetYaw = 0.0f;
            if (dr == -1) targetYaw = 180.0f;
            if (dc == 1)  targetYaw = 90.0f;
            if (dc == -1) targetYaw = 270.0f;

            currentRow = nextR;
            currentCol = nextC;
            targetPos = Vector3{
                (currentCol * cellSize) + offsetX,
                0.0f,
                (currentRow * cellSize) + offsetZ
            };
            isMoving = true;
        } else {
            isAutoNavigating = false;
            reachedGoal = true;
            spawnConfetti(); // ¡Lluvia de confeti al pisar la meta!
        }
        return;
    }

    // ------------------------------------------
    // E. Control Manual (W, A, S, D)
    // ------------------------------------------
    int nextRow = currentRow;
    int nextCol = currentCol;
    float newYaw = yaw;
    bool wantsToMove = false;

    if (IsKeyPressed(KEY_W)) { nextRow -= 1; newYaw = 180.0f; wantsToMove = true; }
    else if (IsKeyPressed(KEY_S)) { nextRow += 1; newYaw = 0.0f;   wantsToMove = true; }
    else if (IsKeyPressed(KEY_A)) { nextCol -= 1; newYaw = 270.0f; wantsToMove = true; }
    else if (IsKeyPressed(KEY_D)) { nextCol += 1; newYaw = 90.0f;  wantsToMove = true; }

    if (wantsToMove && !maze.isWall(nextRow, nextCol)) {
        currentRow = nextRow;
        currentCol = nextCol;
        targetYaw = newYaw;
        targetPos = Vector3{
            (currentCol * cellSize) + offsetX,
            0.0f,
            (currentRow * cellSize) + offsetZ
        };
        isMoving = true;
        if (currentRow == maze.getEnd().first && currentCol == maze.getEnd().second) {
            reachedGoal = true;
            spawnConfetti();
        }
    }
}

void Character::draw(bool visible) {
    // 1. Dibujar Estela Luminosa en el suelo (Idea 4)
    for (const auto& t : trail) {
        Color trailColor = Fade(Color{ 255, 120, 200, 255 }, t.alpha * 0.7f);
        DrawCircle3D(Vector3{ t.position.x, 0.025f, t.position.z }, 0.22f * t.alpha, Vector3{ 1.0f, 0.0f, 0.0f }, 90.0f, trailColor);
    }

    // 2. Dibujar Partículas 3D de Confeti al Ganar (Idea 2)
    if (reachedGoal) {
        for (const auto& p : confetti) {
            DrawCube(p.position, p.size, p.size, p.size, p.color);
        }
    }

    if (!visible) return;

    // Salto festivo al ganar
    float hopY = reachedGoal ? fabsf(sinf(celebrationTimer * 6.0f)) * 0.18f : 0.0f;

    Color ringColor = reachedGoal ? Color{ 255, 230, 80, 240 } : Color{ 255, 215, 0, 180 };
    DrawCircle3D(Vector3{ currentPos.x, 0.02f, currentPos.z }, 0.40f, Vector3{ 1.0f, 0.0f, 0.0f }, 90.0f, ringColor);
    DrawCircle3D(Vector3{ currentPos.x, 0.03f, currentPos.z }, 0.28f, Vector3{ 1.0f, 0.0f, 0.0f }, 90.0f, Color{ 255, 105, 180, 220 });

    Vector3 headPos = Vector3{ currentPos.x, currentPos.y + 0.65f + hopY, currentPos.z };

    // Cuerpo
    DrawCapsule(Vector3{ currentPos.x, currentPos.y + hopY, currentPos.z },
                Vector3{ currentPos.x, currentPos.y + 0.45f + hopY, currentPos.z },
                0.22f, 16, 8, Color{ 180, 60, 200, 255 });

    // Cabeza
    DrawSphere(headPos, 0.22f, Color{ 255, 220, 200, 255 });

    // Orejitas
    DrawSphere(Vector3{ headPos.x - 0.14f, headPos.y + 0.18f, headPos.z }, 0.08f, Color{ 255, 50, 120, 255 });
    DrawSphere(Vector3{ headPos.x + 0.14f, headPos.y + 0.18f, headPos.z }, 0.08f, Color{ 255, 50, 120, 255 });

    // Ojos
    float rad = yaw * DEG2RAD;
    Vector3 forward = { sinf(rad), 0.0f, cosf(rad) };
    Vector3 right = { cosf(rad), 0.0f, -sinf(rad) };

    Vector3 eyeL = Vector3Add(headPos, Vector3Add(Vector3Scale(forward, 0.19f), Vector3Scale(right, -0.07f)));
    Vector3 eyeR = Vector3Add(headPos, Vector3Add(Vector3Scale(forward, 0.19f), Vector3Scale(right, 0.07f)));
    DrawSphere(eyeL, 0.035f, Color{ 30, 20, 40, 255 });
    DrawSphere(eyeR, 0.035f, Color{ 30, 20, 40, 255 });
}