#pragma once

#include "Constants.hpp"
#include "gameobjects/ObjectGeometry.hpp"

struct Pad
{
    RectGeometry geometry;

    float speed = Constants::StartingBallSpeed;
    bool movingLeft = false;
    bool movingRight = false;
};