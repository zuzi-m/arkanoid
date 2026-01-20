#include "GeometryEngine.hpp"

#include "gameobjects/ObjectGeometry.hpp"

#include <SDL3/SDL.h>

void
GeometryEngine::SimulateMovement(CircleGeometry& a_circle, const float a_deltaSeconds) const
{
    if (a_circle.properties.velocity.has_value())
    {
        a_circle.center.x += a_circle.properties.velocity->x * a_deltaSeconds;
        a_circle.center.y += a_circle.properties.velocity->y * a_deltaSeconds;
    }
}

void
GeometryEngine::SimulateMovement(RectGeometry& a_rect, const float a_deltaSeconds) const
{
    if (a_rect.properties.velocity.has_value())
    {
        a_rect.rect.x += a_rect.properties.velocity->x * a_deltaSeconds;
        a_rect.rect.y += a_rect.properties.velocity->y * a_deltaSeconds;
    }
}

bool
GeometryEngine::ProcessCollision(CircleGeometry& a_circle, RectGeometry& a_rect, SDL_FPoint* a_contactPoint_p) const
{
    bool centerIsInsideRect = false;
    const SDL_FPoint closestPoint = getClosestPointOnRect(a_circle, a_rect, centerIsInsideRect);
    const SDL_FPoint distanceToClosestPoint{
        a_circle.center.x - closestPoint.x,
        a_circle.center.y - closestPoint.y
    };

    // Avoding more calculations for obvious non-collisions
    if (!centerIsInsideRect && (SDL_abs(distanceToClosestPoint.x) + SDL_abs(distanceToClosestPoint.y)) > a_circle.radius)
    {
        return false;
    }

    const float distanceSquared = distanceToClosestPoint.x * distanceToClosestPoint.x +
                                    distanceToClosestPoint.y * distanceToClosestPoint.y;
    if (distanceSquared < (a_circle.radius * a_circle.radius))
    {
        if (a_contactPoint_p)
        {
            *a_contactPoint_p = closestPoint;
        }
        
        if (!a_circle.properties.isSolid || !a_rect.properties.isSolid || !a_circle.properties.velocity.has_value())
        {
            return true;
        }

        if (centerIsInsideRect)
        {
            // Push circle out to the edge of the rectangle
            a_circle.center.x = closestPoint.x + (distanceToClosestPoint.x > 0 ? a_circle.radius : -a_circle.radius);
            a_circle.center.y = closestPoint.y + (distanceToClosestPoint.y > 0 ? a_circle.radius : -a_circle.radius);
            return true;
        }

        const float normalLength = SDL_sqrtf(distanceSquared);
        const SDL_FPoint normal{
            distanceToClosestPoint.x / normalLength,
            distanceToClosestPoint.y / normalLength
        };

        SDL_FPoint& velocity = a_circle.properties.velocity.value();
        const float velocityDotNormal = velocity.x * normal.x + velocity.y * normal.y;

        if (velocityDotNormal > 0)
        {
            // Moving away from the surface, no collision
            return false;
        }

        //std::cout << "closest point (" << closestPoint.x << ", " << closestPoint.y << ")\n";
        //std::cout << "distance to closest (" << distanceToClosestPoint.x << ", " << distanceToClosestPoint.y << ")\n";
        //std::cout << "center (" << a_circle.center.x << ", " << a_circle.center.y << ")\n";

        //SDL_FPoint prev = center;
        a_circle.center.x = closestPoint.x + normal.x * a_circle.radius;
        a_circle.center.y = closestPoint.y + normal.y * a_circle.radius;
        velocity.x -= 2.0f * velocityDotNormal * normal.x;
        velocity.y -= 2.0f * velocityDotNormal * normal.y;

        if (SDL_isnan(velocity.x) || SDL_isnan(velocity.y) || SDL_isnan(a_circle.center.x) || SDL_isnan(a_circle.center.y))
        {
            velocity.x = 0.0f;
            velocity.y = 0.0f;
        }

        return true;
    }

    return false;
}

SDL_FPoint
GeometryEngine::getClosestPointOnRect(const CircleGeometry &a_circle, const RectGeometry &a_rect, bool& a_isInsideRect) const
{
    if (SDL_PointInRectFloat(&a_circle.center, &a_rect.rect))
    {
        a_isInsideRect = true;
        SDL_FPoint closestPointsOnEdges[4];

        // Left
        closestPointsOnEdges[0] = {
            a_rect.rect.x,
            a_circle.center.y
        };
        // Right
        closestPointsOnEdges[1] = {
            a_rect.rect.x + a_rect.rect.w,
            a_circle.center.y
        };
        // Top
        closestPointsOnEdges[2] = {
            a_circle.center.x,
            a_rect.rect.y
        };
        // Bottom
        closestPointsOnEdges[3] = {
            a_circle.center.x,
            a_rect.rect.y + a_rect.rect.h
        };
        
        int closestPointIndex = 0;
        float closestDistanceSquared = 0.0f;
        for (int i = 0; i < SDL_arraysize(closestPointsOnEdges); i++)
        {
            const SDL_FPoint& difference{
                a_circle.center.x - closestPointsOnEdges[i].x,
                a_circle.center.y - closestPointsOnEdges[i].y
            };
            const float currentDistanceSquared = (difference.x * difference.x) + (difference.y * difference.y);

            if (i == 0 || currentDistanceSquared < closestDistanceSquared)
            {
                closestPointIndex = i;
                closestDistanceSquared = currentDistanceSquared;
            }
        }
        
        return closestPointsOnEdges[closestPointIndex];
    }

    a_isInsideRect = false;
    return {
        SDL_clamp(a_circle.center.x, a_rect.rect.x, a_rect.rect.x + a_rect.rect.w),
        SDL_clamp(a_circle.center.y, a_rect.rect.y, a_rect.rect.y + a_rect.rect.h)
    };
}
