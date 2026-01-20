#pragma once

#include "Constants.hpp"
#include "gameobjects/ObjectGeometry.hpp"

#include <SDL3/SDL.h>

struct Pad
{
    RectGeometry geometry;

    float maxSpeed = Constants::DefaultPadSpeed;
    bool movingLeft = false;
    bool movingRight = false;
};