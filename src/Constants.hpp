#pragma once

#include <SDL3/SDL.h>

namespace Constants
{
    const int WINDOW_WIDTH = 600;
    const int WINDOW_HEIGHT = 800;

    const Uint64 MinDeltaTimeMillis = 1000 / 60;
    const int CircleSegments = 32;

    const float DefaultPadSpeed = 350.0f;
    const float DefaultBallSpeed = 105.0f;
    const float DefaultBallSize = 30.0f;
}