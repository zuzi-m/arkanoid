#include "LevelFactory.hpp"

#include "gameobjects/Level.hpp"

namespace
{

const float LevelBoundsMargin = 80.0f;
const float DefaultBrickHeight = 35.0f;
const float DefaultBrickWidth = 70.0f;
const float DefaultBrickSpacing = 15.0f;

const float DefaultPadWidth = 140.0f;
const float DefaultPadHeight = 25.0f;

}

Level
LevelFactory::CreateLevel(const SDL_FRect& a_levelBounds)
{
    Level level;
    level.bounds = a_levelBounds;

    level.bricks.clear();

    const int bricksPerRow = 5;
    const SDL_FPoint brickSize{DefaultBrickWidth, DefaultBrickHeight};

    float yOffset = LevelBoundsMargin;

    for (int row = 0; row < 7; row++)
    {
        switch (row)
        {
        case 0:
            addBrickRow(level, yOffset, DefaultBrickSpacing, brickSize, {BrickKind::NormalScore,BrickKind::LowScore, BrickKind::NormalScore, BrickKind::HighScore, BrickKind::NormalScore, BrickKind::LowScore, BrickKind::NormalScore});
            break;

        case 1:
            addBrickRow(level, yOffset, DefaultBrickSpacing, brickSize, {BrickKind::NormalScore, BrickKind::HighScore, BrickKind::HighScore, BrickKind::HighScore, BrickKind::NormalScore});
            break;

        case 2:
            addBrickRow(level, yOffset, DefaultBrickSpacing, brickSize, {BrickKind::HighScore, std::nullopt, std::nullopt, std::nullopt, BrickKind::HighScore});
            break;

        case 3:
            addBrickRow(level, yOffset, DefaultBrickSpacing, brickSize, {BrickKind::NormalScore, BrickKind::NormalScore, std::nullopt, BrickKind::NormalScore, BrickKind::NormalScore});
            break;

        case 4:
            addBrickRow(level, yOffset, DefaultBrickSpacing, brickSize, {BrickKind::LowScore, BrickKind::LowScore, std::nullopt, BrickKind::LowScore, BrickKind::LowScore});
            break;

        case 5:
            addBrickRow(level, yOffset, DefaultBrickSpacing, brickSize, {BrickKind::LowScore, BrickKind::LowScore, std::nullopt, BrickKind::LowScore, BrickKind::LowScore});
            break;

        default:
            addBrickRow(level, yOffset, DefaultBrickSpacing, brickSize, {BrickKind::LowScore, BrickKind::LowScore, BrickKind::HighScore, BrickKind::LowScore, BrickKind::LowScore});
            break;
        }

        yOffset += DefaultBrickSpacing + DefaultBrickHeight;
    }

    level.pad.geometry.properties.isSolid = true;
    level.pad.geometry.properties.isVisible = true;
    
    level.pad.geometry.rect.w = DefaultPadWidth;
    level.pad.geometry.rect.h = DefaultPadHeight;
    level.pad.geometry.rect.x = a_levelBounds.x + (a_levelBounds.w / 2.0f) - (level.pad.geometry.rect.w / 2.0);
    level.pad.geometry.rect.y = a_levelBounds.y + a_levelBounds.h - LevelBoundsMargin - level.pad.geometry.rect.h;
    
    level.pad.speed = Constants::StartingPadSpeed;

    level.ball.geometry.properties.isSolid = true;
    level.ball.geometry.properties.isVisible = true;
    level.ball.geometry.radius = Constants::StartingBallSize / 2.0f;
    level.paused = false;
    level.balls = 3;
    level.score = 0;

    return level;
}

void
LevelFactory::addBrickRow(Level &a_level,
                          const float a_yOffset,
                          const float a_spacing,
                          const SDL_FPoint& a_brickSize,
                          const std::vector<std::optional<BrickKind>> a_bricksToAdd) const
{
    const float totalRowWidth = a_brickSize.x * a_bricksToAdd.size() + (a_bricksToAdd.size() - 1) * a_spacing;
    float currentX = a_level.bounds.x + a_level.bounds.w / 2.0f - totalRowWidth / 2.0f;

    for (const auto& brickType : a_bricksToAdd)
    {
        if (brickType.has_value())
        {
            Brick brick;
            brick.geometry.properties.isSolid = true;
            brick.geometry.properties.isVisible = true;
            brick.geometry.rect.x = currentX;
            brick.geometry.rect.y = a_level.bounds.y + a_yOffset;
            brick.geometry.rect.w = a_brickSize.x;
            brick.geometry.rect.h = a_brickSize.y;
            brick.hitPoints = 1;
            brick.kind = brickType.value();

            a_level.bricks.push_back(brick);
        }

        currentX += a_brickSize.x + a_spacing;
    }
}
