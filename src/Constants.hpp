#pragma once

#include <SDL3/SDL.h>

namespace Constants
{
    const int WINDOW_WIDTH = 600;
    const int WINDOW_HEIGHT = 800;

    const Uint64 MinDeltaTimeMillis = 1000 / 60;
    const int CircleSegments = 32;

    const int StartingBallCount = 3;
    const float StartingBallSize = 30.0f;
    const float StartingBallSpeed = 300.0f;
    const float StartingPadSpeed = 450.0f;
}