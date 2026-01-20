#include "LevelController.hpp"

#include "gameobjects/Brick.hpp"
#include "GeometryEngine.hpp"

LevelController::LevelController(std::shared_ptr<GeometryEngine> a_geometryEngine_sp, const Level& a_level)
    : m_geometryEngine_sp(a_geometryEngine_sp)
    , m_level(a_level)
    , m_gameOver(false)
{
}

const Level&
LevelController::GetLevel() const
{
    return m_level;
}

bool
LevelController::GameOver() const
{
    return m_gameOver;
}

SDL_AppResult
LevelController::Iterate(const float a_deltaTimeSec)
{
    if (m_level.paused || m_gameOver)
    {
        return SDL_APP_CONTINUE;
    }

    // Simulate movement
    m_geometryEngine_sp->SimulateMovement(m_level.pad.geometry, a_deltaTimeSec);
    m_level.pad.geometry.rect.x = SDL_clamp(m_level.pad.geometry.rect.x, m_level.bounds.x, m_level.bounds.x + m_level.bounds.w - m_level.pad.geometry.rect.w);
    
    if (m_level.ballLaunched)
    {
        m_geometryEngine_sp->SimulateMovement(m_level.ball.geometry, a_deltaTimeSec);
    }
    else
    {
        resetBall();
    }

    SDL_FPoint ballCenterMin{
        m_level.bounds.x + m_level.ball.geometry.radius,
        m_level.bounds.y + m_level.ball.geometry.radius
    };
    SDL_FPoint ballCenterMax{
        ballCenterMin.x + m_level.bounds.w - m_level.ball.geometry.radius * 2.0f,
        ballCenterMin.y + m_level.bounds.h
    };

    if (m_level.ball.geometry.center.x < ballCenterMin.x)
    {
        m_level.ball.geometry.center.x = ballCenterMin.x;
        m_level.ball.geometry.properties.velocity->x *= -1.0f;
    }
    else if (m_level.ball.geometry.center.x > ballCenterMax.x)
    {
        m_level.ball.geometry.center.x = ballCenterMax.x;
        m_level.ball.geometry.properties.velocity->x *= -1.0f;
    }

    if (m_level.ball.geometry.center.y < ballCenterMin.y)
    {
        m_level.ball.geometry.center.y = ballCenterMin.y;
        m_level.ball.geometry.properties.velocity->y *= -1.0f;
    }
    else if (m_level.ball.geometry.center.y > ballCenterMax.y)
    {
        m_level.ballLaunched = false;
        m_level.balls--;
        resetBall();
    }
    
    // Simulate collisions
    bounceBallFromPad();

    auto it = m_level.bricks.begin();
    while (it != m_level.bricks.end())
    {
        if (m_geometryEngine_sp->ProcessCollision(m_level.ball.geometry, it->geometry, nullptr))
        {
            if (it->hitPoints > 0)
            {
                it->hitPoints--;
            }

            if (it->hitPoints == 0)
            {
                m_level.score += getBrickScore(*it);
                it = m_level.bricks.erase(it);
            }
        }
        else
        {
            ++it;
        }
    }

    if (m_level.bricks.empty() || m_level.balls < 0)
    {
        m_gameOver = true;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult
LevelController::HandleKeyboardEvent(const SDL_KeyboardEvent& a_keyEvent)
{
    const bool isPressed = a_keyEvent.type == SDL_EVENT_KEY_DOWN;
    switch (a_keyEvent.key)
    {
        case SDLK_LEFT:
            updatePadMovement(isPressed, m_level.pad.movingRight);
            break;

        case SDLK_RIGHT:
            updatePadMovement(m_level.pad.movingLeft, isPressed);
            break;

        case SDLK_SPACE:
            if (isPressed && !m_level.paused && !m_level.ballLaunched)
            {
                launchBall();
            }
            break;

        case SDLK_P:
            if (isPressed)
            {
                m_level.paused = !m_level.paused;
            }
            break;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult
LevelController::HandleMouseMotionEvent(const SDL_MouseMotionEvent& a_mouseMotionEvent, const SDL_FPoint a_levelMousePosition)
{
    return SDL_APP_CONTINUE;
}

SDL_AppResult
LevelController::HandleMouseButtonEvent(const SDL_MouseButtonEvent& a_mouseButtonEvent, const SDL_FPoint a_levelMousePosition)
{
    // TODO: this is only for debug
    if (a_mouseButtonEvent.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        if (a_mouseButtonEvent.button == SDL_BUTTON_LEFT)
        {
            m_level.ball.geometry.center.x = a_levelMousePosition.x;
            m_level.ball.geometry.center.y = a_levelMousePosition.y;
        }
        else if (a_mouseButtonEvent.button == SDL_BUTTON_RIGHT && m_level.ball.geometry.properties.velocity.has_value())
        {
            m_level.ball.geometry.properties.velocity->x = a_levelMousePosition.x - (m_level.ball.geometry.center.x);
            m_level.ball.geometry.properties.velocity->y = a_levelMousePosition.y - (m_level.ball.geometry.center.y);
        }
    }

    return SDL_APP_CONTINUE;
}

void
LevelController::launchBall()
{
    resetBall();

    m_level.ball.geometry.properties.velocity = SDL_FPoint{
        SDL_randf() * Constants::DefaultBallSpeed * 2 - Constants::DefaultBallSpeed,
        -Constants::DefaultBallSpeed
    };
    m_level.ballLaunched = true;
}

void
LevelController::resetBall()
{
    m_level.ball.geometry.center.x = m_level.pad.geometry.rect.x + (m_level.pad.geometry.rect.w / 2.0f);
    m_level.ball.geometry.center.y = m_level.pad.geometry.rect.y - m_level.ball.geometry.radius;
    m_level.ball.geometry.properties.velocity.reset();
}

void
LevelController::updatePadMovement(const bool a_moveLeft, const bool a_moveRight)
{
    m_level.pad.movingLeft = a_moveLeft;
    m_level.pad.movingRight = a_moveRight;

    if (a_moveLeft == a_moveRight)
    {
        m_level.pad.geometry.properties.velocity.reset();
    }
    else
    {
        const float direction = a_moveLeft ? -1.0f : 1.0f;
        m_level.pad.geometry.properties.velocity = SDL_FPoint{ direction * m_level.pad.maxSpeed, 0.0f };
    }
}

void
LevelController::bounceBallFromPad()
{
    SDL_FPoint padContactPoint;
    if (m_geometryEngine_sp->ProcessCollision(m_level.ball.geometry, m_level.pad.geometry, &padContactPoint))
    {
        if (padContactPoint.y == m_level.pad.geometry.rect.y)
        {
            const float padPosition = (padContactPoint.x - m_level.pad.geometry.rect.x) / m_level.pad.geometry.rect.w;
            const float maxBallBounceAngleRad = 1.4f; // Approx 80 degrees
            const float angle = (-maxBallBounceAngleRad * padPosition) + (maxBallBounceAngleRad * (1.0f - padPosition));
            m_level.ball.geometry.properties.velocity = {
                -Constants::DefaultBallSpeed * SDL_cosf(angle - SDL_PI_F / 2.0f),
                Constants::DefaultBallSpeed * SDL_sinf(angle - SDL_PI_F / 2.0f),
            };
        }
    }
}

Uint32
LevelController::getBrickScore(const Brick &a_brick)
{
    switch (a_brick.kind)
    {
    case BrickKind::HighScore:
        return 300;

    case BrickKind::NormalScore:
        return 200;

    case BrickKind::LowScore:
        return 100;
    }

    return 0;
}
