//
// Created by Novak on 29/03/2020.
//

#ifndef EAGLE_CAMERACONTROLLER_H
#define EAGLE_CAMERACONTROLLER_H

#include <eagle/engine/EngineGlobalDefinitions.h>

EG_ENGINE_BEGIN

struct CameraController {
    float speed = 20.0f;
    float sensitivity = 0.05f;
    float minimumX = -360.0f;
    float maximumX = 360.0f;
    float minimumY = -80.0f;
    float maximumY = 90.0f;
    float frameCounterTime = 0.1f;
    float frameCounter;

    std::vector<float> rotArrayX;
    std::vector<float> rotArrayY;

    float rotationX;
    float rotationY;
    float rotAverageX;
    float rotAverageY;

    glm::quat originalRotation;
    bool dragging = false;
};

EG_ENGINE_END

#endif //EAGLE_CAMERACONTROLLER_H
