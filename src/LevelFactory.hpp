#pragma once

struct Level;
struct SDL_FPoint;
struct SDL_FRect;

#include <optional>
#include <vector>

enum class BrickKind;

class LevelFactory
{
public:
    virtual ~LevelFactory() = default;

    Level CreateLevel(const SDL_FRect& a_levelBounds);

private:
    void addBrickRow(Level& a_level,
                     const float a_yOffset,
                     const float a_spacing,
                     const SDL_FPoint& a_brickSize,
                     const std::vector<std::optional<BrickKind>> a_bricksToAdd) const;
};