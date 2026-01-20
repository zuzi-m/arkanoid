#pragma once

struct CircleGeometry;
struct RectGeometry;
struct SDL_FPoint;

class GeometryEngine
{
public:
    virtual ~GeometryEngine() = default;

    void SimulateMovement(CircleGeometry& a_circle, const float a_deltaSeconds) const;
    void SimulateMovement(RectGeometry& a_rect, const float a_deltaSeconds) const;
    
    bool ProcessCollision(CircleGeometry& a_circle, RectGeometry& a_rect, SDL_FPoint* a_contactPoint_p) const;

    SDL_FPoint RotateVector(const SDL_FPoint a_vector, const float a_angleRad) const;

private:
    SDL_FPoint getClosestPointOnRect(const CircleGeometry& a_circle, const RectGeometry& a_rect, bool& a_isInsideRect) const;
};