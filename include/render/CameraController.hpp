#pragma once
#include "raylib.h"

enum class CameraState {
    ORBITAL = 1,
    THIRD_PERSON = 2,
    FIRST_PERSON = 3
};

class CameraController {
private:
    Camera3D camera = { 0 };
    CameraState currentState = CameraState::ORBITAL;

    float thirdPersonDist = 2.8f;
    float thirdPersonHeight = 1.6f;

    float firstPersonEyeHeight = 0.65f;
    float mouseSensitivity = 0.003f;
    float cameraPitch = 0.0f;
    float cameraYaw = 0.0f;

    float currentMapMaxDim = 10.0f; // Para controlar límites de zoom

public:
    CameraController();

    void setMode(CameraState state);
    CameraState getMode() const { return currentState; }

    // Auto-ajusta la cámara al tamaño de cualquier mapa
    void autoFit(int rows, int cols, float cellSize);

    void update(float deltaTime, Vector3 targetPos, float targetYaw);
    const Camera3D& getCamera() const { return camera; }

    bool shouldRenderCharacter() const {
        return currentState != CameraState::FIRST_PERSON;
    }
};