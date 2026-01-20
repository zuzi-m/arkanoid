#pragma once

#include <SDL3/SDL.h>

#include <memory>

class GeometryEngine;
class LevelController;
class LevelFactory;
class Renderer;

class Game
{
public:
    explicit Game(std::shared_ptr<Renderer> a_renderer_sp,
                  std::shared_ptr<LevelFactory> a_levelFactory_sp,
                  std::shared_ptr<GeometryEngine> a_geometryEngine_sp);
    virtual ~Game() = default;

    SDL_AppResult Iterate();
    SDL_AppResult HandleInput(void* a_appstate_p, SDL_Event* a_event_p);

private:
    std::shared_ptr<Renderer> m_renderer_sp;
    std::shared_ptr<LevelFactory> m_levelFactory_sp;
    std::shared_ptr<GeometryEngine> m_geometryEngine_sp;

    std::shared_ptr<LevelController> m_currentLevel_sp;
    Uint64 m_lastTimeMillis;
};
