#include "Game.hpp"

#include "Constants.hpp"
#include "GeometryEngine.hpp"
#include "LevelController.hpp"
#include "LevelFactory.hpp"
#include "Renderer.hpp"

Game::Game(std::shared_ptr<Renderer> a_renderer_sp,
           std::shared_ptr<LevelFactory> a_levelFactory_sp,
           std::shared_ptr<GeometryEngine> a_geometryEngine_sp)
    : m_renderer_sp(a_renderer_sp)
    , m_levelFactory_sp(a_levelFactory_sp)
    , m_geometryEngine_sp(a_geometryEngine_sp)
    , m_currentLevel_sp(nullptr)
    , m_lastTimeMillis(0)
{
}

SDL_AppResult
Game::Iterate()
{
    const Uint64 now = SDL_GetTicks();
    const Uint64 deltaMillis = now - m_lastTimeMillis;
    
    if (deltaMillis < Constants::MinDeltaTimeMillis)
    {
        return SDL_APP_CONTINUE;
    }

    m_lastTimeMillis = now;
    const float deltaSeconds = deltaMillis / 1000.0f;

    if (!m_currentLevel_sp)
    {
        m_renderer_sp->RenderTitleScreen();
    }

    if (m_currentLevel_sp)
    {
        if (m_currentLevel_sp->GameOver())
        {
            m_renderer_sp->RenderFinalScore(m_currentLevel_sp->GetLevel().balls >= 0, m_currentLevel_sp->GetLevel().score);
        }
        else
        {
            m_currentLevel_sp->Iterate(deltaSeconds);
            m_renderer_sp->RenderLevel(m_currentLevel_sp->GetLevel());
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult
Game::HandleInput(void* a_appstate_p, SDL_Event* a_event_p)
{
    if (a_event_p->type == SDL_EVENT_QUIT || (a_event_p->type == SDL_EVENT_KEY_DOWN && a_event_p->key.key == SDLK_ESCAPE))
    {
        return SDL_APP_SUCCESS;
    }

    if (m_currentLevel_sp && !m_currentLevel_sp->GameOver())
    {
        switch (a_event_p->type)
        {
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
                m_currentLevel_sp->HandleKeyboardEvent(a_event_p->key);
                break;

            case SDL_EVENT_MOUSE_MOTION:
                m_currentLevel_sp->HandleMouseMotionEvent(a_event_p->motion, m_renderer_sp->WindowToLevelPosition({a_event_p->motion.x, a_event_p->motion.y}));
                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                m_currentLevel_sp->HandleMouseButtonEvent(a_event_p->button, m_renderer_sp->WindowToLevelPosition({a_event_p->button.x, a_event_p->button.y}));
                break;
        }
    }
    else if (a_event_p->type == SDL_EVENT_KEY_DOWN || a_event_p->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (!m_currentLevel_sp)
        {
            m_currentLevel_sp = std::make_shared<LevelController>(m_geometryEngine_sp, m_levelFactory_sp->CreateLevel(m_renderer_sp->LevelBounds()));
        }
        else
        {
            return SDL_APP_SUCCESS;
        }
    }

    return SDL_APP_CONTINUE;
}