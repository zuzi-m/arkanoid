#pragma once

#include "gameobjects/Level.hpp"

#include <SDL3/SDL.h>

#include <memory>

struct Brick;
class GeometryEngine;

class LevelController
{
public:
    explicit LevelController(std::shared_ptr<GeometryEngine> a_geometryEngine_sp, const Level& a_level);
    virtual ~LevelController() = default;

    const Level& GetLevel() const;
    bool GameOver() const;

    SDL_AppResult Iterate(const float a_deltaTimeSec);

    SDL_AppResult HandleKeyboardEvent(const SDL_KeyboardEvent& a_keyEvent);
    SDL_AppResult HandleMouseMotionEvent(const SDL_MouseMotionEvent& a_mouseMotionEvent, const SDL_FPoint a_levelMousePosition);
    SDL_AppResult HandleMouseButtonEvent(const SDL_MouseButtonEvent& a_mouseButtonEvent, const SDL_FPoint a_levelMousePosition);

private:
    void launchBall();
    void resetBall();
    void updatePadMovement(const bool a_moveLeft, const bool a_moveRight);
    void bounceBallFromPad();
    Uint32 getBrickScore(const Brick& a_brick);

private:
    std::shared_ptr<GeometryEngine> m_geometryEngine_sp;
    Level m_level;
    bool m_gameOver;
};