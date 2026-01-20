#pragma once

#include "gameobjects/ObjectGeometry.hpp"

enum class BrickKind
{
    LowScore,
    NormalScore,
    HighScore,
    Solid
};

struct Brick
{
    RectGeometry geometry;
    
    BrickKind kind = BrickKind::NormalScore;
    int hitPoints = 1;
};