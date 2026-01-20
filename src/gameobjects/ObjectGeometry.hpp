#pragma once

#include <SDL3/SDL.h>

#include <optional>

struct GeometryProperties
{
    bool isSolid = false;
    bool isVisible = false;
    std::optional<SDL_FPoint> velocity;
};

struct CircleGeometry
{
    GeometryProperties properties;
    SDL_FPoint center;
    float radius = 0.0f;
};

struct RectGeometry
{
    GeometryProperties properties;
    SDL_FRect rect;
};