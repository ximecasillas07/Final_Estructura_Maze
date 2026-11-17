#include "render/CameraController.hpp"
#include "raymath.h"
#include <cmath>
#include <algorithm>

CameraController::CameraController() {
    camera.position = Vector3{ 0.0f, 16.0f, 0.0f };
    camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
    camera.up = Vector3{ 0.0f, 0.0f, -1.0f };
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    currentState = CameraState::ORBITAL;
}

void CameraController::autoFit(int rows, int cols, float cellSize) {
    currentMapMaxDim = static_cast<float>(std::max(rows, cols)) * cellSize;

    // Altura ideal para abarcar todo el laberinto con margen
    float fovRad = (camera.fovy * 0.5f) * DEG2RAD;
    float optimalHeight = (currentMapMaxDim * 0.85f) / tanf(fovRad);

    if (currentState == CameraState::ORBITAL) {
        camera.position = Vector3{ 0.0f, optimalHeight, 0.0f };
        camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
        camera.up = Vector3{ 0.0f, 0.0f, -1.0f };
    }
}

void CameraController::setMode(CameraState state) {
    currentState = state;
    if (currentState == CameraState::ORBITAL) {
        camera.fovy = 45.0f;
        camera.up = Vector3{ 0.0f, 0.0f, -1.0f };
        float fovRad = (camera.fovy * 0.5f) * DEG2RAD;
        camera.position.y = (currentMapMaxDim * 0.85f) / tanf(fovRad);
        camera.position.x = 0.0f;
        camera.position.z = 0.0f;
        camera.target = Vector3{ 0.0f, 0.0f, 0.0f };
    } else if (currentState == CameraState::THIRD_PERSON) {
        camera.fovy = 60.0f;
        camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    } else if (currentState == CameraState::FIRST_PERSON) {
        camera.fovy = 75.0f;
        camera.up = Vector3{ 0.0f, 1.0f, 0.0f };
    }
}

void CameraController::update(float deltaTime, Vector3 targetPos, float targetYaw) {
    if (currentState == CameraState::ORBITAL) {
        // Zoom que permite alejarte todo lo que quieras
        float wheel = GetMouseWheelMove();
        if (wheel != 0.0f) {
            float zoomSpeed = std::max(4.0f, currentMapMaxDim * 0.18f);
            camera.position.y -= wheel * zoomSpeed;
            camera.position.y = std::clamp(camera.position.y, 4.0f, 800.0f);
        }
    }
    else if (currentState == CameraState::THIRD_PERSON) {
        float rad = targetYaw * DEG2RAD;
        Vector3 forward = { sinf(rad), 0.0f, cosf(rad) };

        Vector3 desiredPos = Vector3Subtract(targetPos, Vector3Scale(forward, thirdPersonDist));
        desiredPos.y += thirdPersonHeight;

        camera.position = Vector3Lerp(camera.position, desiredPos, 12.0f * deltaTime);
        camera.target = Vector3Add(targetPos, Vector3{ 0.0f, 0.5f, 0.0f });
    }
    else if (currentState == CameraState::FIRST_PERSON) {
        Vector3 eyePos = Vector3Add(targetPos, Vector3{ 0.0f, firstPersonEyeHeight, 0.0f });
        camera.position = eyePos;

        float diff = targetYaw - cameraYaw;
        while (diff < -180.0f) diff += 360.0f;
        while (diff > 180.0f) diff -= 360.0f;
        cameraYaw += diff * 12.0f * deltaTime;

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            Vector2 mouseDelta = GetMouseDelta();
            cameraPitch -= mouseDelta.y * mouseSensitivity;
            cameraPitch = std::clamp(cameraPitch, -0.9f, 0.9f);
        }

        float rad = cameraYaw * DEG2RAD;
        Vector3 lookDir = {
            sinf(rad) * cosf(cameraPitch),
            sinf(cameraPitch),
            cosf(rad) * cosf(cameraPitch)
        };
        camera.target = Vector3Add(camera.position, lookDir);
    }
}