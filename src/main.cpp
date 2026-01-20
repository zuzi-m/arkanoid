#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "Game.hpp"
#include "GeometryEngine.hpp"
#include "LevelFactory.hpp"
#include "Renderer.hpp"

struct {
    std::shared_ptr<Renderer> renderer_sp;
    std::shared_ptr<LevelFactory> levelFactory_sp;
    std::shared_ptr<GeometryEngine> geometryEngine_sp;
    std::shared_ptr<Game> game_sp;
} App;

SDL_AppResult
SDL_AppInit(void **appstate, int argc, char *argv[])
{
    App.renderer_sp = std::make_shared<Renderer>();
    App.levelFactory_sp = std::make_shared<LevelFactory>();
    App.geometryEngine_sp = std::make_shared<GeometryEngine>();
    App.game_sp = std::make_shared<Game>(App.renderer_sp, App.levelFactory_sp, App.geometryEngine_sp);

    SDL_SetAppMetadata("Arkanoid demo game", "0.0", "com.github.zuzi-m.arkanoid");

    return App.renderer_sp->Init();
}

SDL_AppResult
SDL_AppEvent(void *appstate, SDL_Event *event)
{
    return App.game_sp->HandleInput(appstate, event);
}

SDL_AppResult
SDL_AppIterate(void *appstate)
{
    return App.game_sp->Iterate();
}

void
SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
    App.game_sp.reset();
    App.renderer_sp.reset();
}