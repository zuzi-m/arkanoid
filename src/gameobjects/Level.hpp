#pragma once

#include "Ball.hpp"
#include "Brick.hpp"
#include "Pad.hpp"

#include <vector>

struct Level {
    SDL_FRect bounds;
    Pad pad;
    std::vector<Brick> bricks;
    Ball ball;

    bool paused = false;
    bool ballLaunched = false;
    int balls = 3;
    Uint32 score = 0;
};