#include "Renderer.hpp"

#include "Constants.hpp"
#include "gameobjects/Level.hpp"

#include <sstream>

namespace
{
    const float WindowMargin = 60.0f;

    const SDL_Color ClearColor      {0, 0, 0, SDL_ALPHA_OPAQUE};
    const SDL_Color LevelBoundsColor{255, 255, 255, 25};
    const SDL_Color TextColor       {255, 255, 255, SDL_ALPHA_OPAQUE};
    const SDL_Color PauseColor      {0, 0, 0, 175};

    const SDL_Color PadColor        {0x84, 0xa9, 0x8c, SDL_ALPHA_OPAQUE};
    const SDL_Color BallColor       {0xea, 0xe2, 0xb7, SDL_ALPHA_OPAQUE};
    const SDL_Color BrickHighColor  {0xd6, 0x28, 0x28, SDL_ALPHA_OPAQUE};
    const SDL_Color BrickNormalColor{0xf7, 0x7f, 0x00, SDL_ALPHA_OPAQUE};
    const SDL_Color BrickLowColor   {0xfc, 0xbf, 0x49, SDL_ALPHA_OPAQUE};
    const SDL_Color BrickSolidColor {105, 105, 105, SDL_ALPHA_OPAQUE};
    const SDL_Color DebugColor      {255, 0, 0, SDL_ALPHA_OPAQUE};
}

SDL_AppResult
Renderer::Init()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Arkanoid", Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &m_window_p, &m_renderer_p))
    {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    
    if (!SDL_SetRenderLogicalPresentation(m_renderer_p, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX))
    {
        SDL_Log("Couldn't set logical presentation: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_SetRenderDrawBlendMode(m_renderer_p, SDL_BLENDMODE_BLEND))
    {
        SDL_Log("Couldn't set alpha blend mode: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

void
Renderer::RenderTitleScreen()
{
    setDrawColor(ClearColor);
    SDL_RenderClear(m_renderer_p);

    renderUiRectWithText(LevelBounds(), "Press any key to start");

    SDL_RenderPresent(m_renderer_p);
}

void Renderer::RenderLevel(const Level &a_level)
{
    setDrawColor(ClearColor);
    SDL_RenderClear(m_renderer_p);

    setDrawColor(LevelBoundsColor);
    SDL_RenderFillRect(m_renderer_p, &a_level.bounds);

    for (const Brick& brick : a_level.bricks)
    {
        renderBrick(brick);
    }

    if (a_level.pad.geometry.properties.isVisible)
    {
        setDrawColor(PadColor);
        SDL_RenderFillRect(m_renderer_p, &a_level.pad.geometry.rect);
    }

    if (a_level.ball.geometry.properties.isVisible)
    {
        renderCircle(a_level.ball.geometry.center, a_level.ball.geometry.radius, BallColor);
    }

    if (false)
    {
        renderBallDebugLines(a_level.ball);
    }

    if (a_level.paused)
    {
        setDrawColor(PauseColor);
        SDL_RenderFillRect(m_renderer_p, &a_level.bounds);
    }

    std::stringstream s;
    s << "Balls: " << a_level.balls << "\tScore: " << a_level.score;
    const std::string topText = s.str();
    const std::string bottomText = a_level.paused ? "Game paused. Press P to resume." : "P: pause, SPACE: launch ball, LEFT,RIGHT: move pad";

    SDL_FRect uiRect;
    uiRect.x = 0.0f;
    uiRect.y = 0.0f;
    uiRect.w = Constants::WINDOW_WIDTH;
    uiRect.h = WindowMargin;
    renderUiRectWithText(uiRect, topText);
    uiRect.y = Constants::WINDOW_HEIGHT - WindowMargin;
    renderUiRectWithText(uiRect, bottomText);

    SDL_RenderPresent(m_renderer_p);
}

void
Renderer::RenderFinalScore(const bool a_levelCleared, const Uint32 a_score)
{
    setDrawColor(ClearColor);
    SDL_RenderClear(m_renderer_p);

    std::stringstream s;
    if (a_levelCleared)
    {
        s << "You won! ";
    }
    else
    {
        s << "Game over! ";
    }
    s << "Final score: " << a_score;
    renderUiRectWithText(LevelBounds(), s.str());

    SDL_RenderPresent(m_renderer_p);
}

SDL_FRect
Renderer::LevelBounds() const
{
    SDL_FRect levelBounds;
    levelBounds.x = 0.0f;
    levelBounds.y = WindowMargin;
    levelBounds.w = Constants::WINDOW_WIDTH;
    levelBounds.h = Constants::WINDOW_HEIGHT - WindowMargin - WindowMargin;

    return levelBounds;
}

SDL_FPoint
Renderer::WindowToLevelPosition(const SDL_FPoint &a_windowPosition) const
{
    int windowW, windowH;
    SDL_GetWindowSize(m_window_p, &windowW, &windowH);

    SDL_FPoint levelPosition{
        (float)a_windowPosition.x / windowW * Constants::WINDOW_WIDTH,
        (float)a_windowPosition.y / windowH * Constants::WINDOW_HEIGHT
    };

    return levelPosition;
}

void
Renderer::setDrawColor(const SDL_Color& a_color)
{
    SDL_SetRenderDrawColor(m_renderer_p, a_color.r, a_color.g, a_color.b, a_color.a);
}

void
Renderer::renderCircle(const SDL_FPoint& a_center, const float a_radius, const SDL_Color& a_color)
{
    const SDL_FColor ballFColor{a_color.r / 255.0f, a_color.g / 255.0f, a_color.b / 255.0f, a_color.a / 255.0f};

    SDL_Vertex points[Constants::CircleSegments * 3];

    for (int i = 0; i < Constants::CircleSegments; i++)
    {
        float theta = (float)i / (float)Constants::CircleSegments * 2.0f * SDL_PI_F;
        points[i*3].position.x = a_center.x + a_radius * SDL_cosf(theta);
        points[i*3].position.y = a_center.y + a_radius * SDL_sinf(theta);
        points[i*3].color = ballFColor;

        points[i*3 + 1].position = a_center;
        points[i*3 + 1].color = ballFColor;

        const int prevIndex = (i == 0) ? (Constants::CircleSegments - 1) : i - 1;
        points[prevIndex*3 + 2] = points[i*3];
    }
    SDL_RenderGeometry(m_renderer_p, nullptr, points, Constants::CircleSegments * 3, nullptr, 0);
}

void
Renderer::renderUiRectWithText(const SDL_FRect& a_rect, const std::string& a_text)
{
    setDrawColor(ClearColor);
    SDL_RenderFillRect(m_renderer_p, &a_rect);

    setDrawColor(TextColor);
    SDL_RenderDebugText(m_renderer_p, a_rect.x + WindowMargin, a_rect.y + a_rect.h / 2.0f, a_text.c_str());
}

void
Renderer::renderBrick(const Brick& a_brick)
{
    if (!a_brick.geometry.properties.isVisible)
    {
        return;
    }

    SDL_Color brickColor;
    switch (a_brick.kind)
    {
    case BrickKind::HighScore:
        brickColor = BrickHighColor;
        break;

    case BrickKind::NormalScore:
        brickColor = BrickNormalColor;
        break;

    case BrickKind::LowScore:
        brickColor = BrickLowColor;
        break;

    case BrickKind::Solid:
        brickColor = BrickSolidColor;
        break;
    }

    setDrawColor(brickColor);
    SDL_RenderFillRect(m_renderer_p, &a_brick.geometry.rect);
}

void
Renderer::renderBallDebugLines(const Ball& a_ball)
{
    setDrawColor(DebugColor);

    const SDL_FPoint ballVelocity = a_ball.geometry.properties.velocity.value_or(SDL_FPoint{0.0f, 0.0f});
    const float velocityLength = SDL_sqrtf(ballVelocity.x * ballVelocity.x + ballVelocity.y * ballVelocity.y);
    if (velocityLength > 0.0f)
    {
        SDL_FPoint velocityNormalized{
            ballVelocity.x / velocityLength,
            ballVelocity.y / velocityLength
        };
        SDL_FPoint line1Start{
            a_ball.geometry.center.x + velocityNormalized.y * a_ball.geometry.radius,
            a_ball.geometry.center.y + -velocityNormalized.x * a_ball.geometry.radius
        };
        SDL_FPoint line2Start{
            a_ball.geometry.center.x + -velocityNormalized.y * a_ball.geometry.radius,
            a_ball.geometry.center.y + velocityNormalized.x * a_ball.geometry.radius
        };
        SDL_RenderLine(m_renderer_p, line1Start.x, line1Start.y, line1Start.x + ballVelocity.x, line1Start.y + ballVelocity.y);
        SDL_RenderLine(m_renderer_p, line2Start.x, line2Start.y, line2Start.x + ballVelocity.x, line2Start.y + ballVelocity.y);
    }
}
