#pragma once

#include <SDL3/SDL.h>

#include <memory>

struct Ball;
struct Brick;
struct Level;

class Renderer {
public:
    virtual ~Renderer() = default;

    SDL_AppResult Init();

    void RenderTitleScreen();
    void RenderLevel(const Level& a_level);
    void RenderFinalScore(const bool a_levelCleared, const Uint32 a_score);
    
    SDL_FRect LevelBounds() const;
    SDL_FPoint WindowToLevelPosition(const SDL_FPoint& a_windowPosition) const;

private:
    void setDrawColor(const SDL_Color& a_color);
    void renderCircle(const SDL_FPoint& a_center, const float a_radius, const SDL_Color& a_color);
    void renderUiRectWithText(const SDL_FRect& a_rect, const std::string& a_text);
    void renderBrick(const Brick& a_brick);
    void renderBallDebugLines(const Ball& a_ball);

private:
    SDL_Window* m_window_p;
    SDL_Renderer* m_renderer_p;
};