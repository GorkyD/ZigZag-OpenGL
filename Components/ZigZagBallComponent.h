#pragma once

#include "Math/Vector3.h"

struct ZigZagBallComponent
{
    Vector3 direction = {0.0f, 0.0f, 1.0f};

    float speed = 4.5f;
    float fallSpeed = 0.0f;

    bool alive = true;
};
