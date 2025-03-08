#pragma once

// #include <glm/glm.hpp>
#include <array>

class Vec2
{
public:
    static const Vec2 LOWEST;
    static const Vec2 MAX;
    static const Vec2 MIN;
    static const Vec2 NEG_UNIT_X;
    static const Vec2 NEG_UNIT_Y;
    static const Vec2 ONE;
    static const Vec2 UNIT_X;
    static const Vec2 UNIT_Y;
    static const Vec2 ZERO;

    static Vec2 bilinear(const Vec2 &, const Vec2 &, const Vec2 &, const Vec2 &, float, float);

    static Vec2 bilinear(const std::array<Vec2, 4ul> &, float, float);

    float x;
    float y;

    Vec2(float _x, float _y) :
        x(_x), y(_y) {}

    Vec2() :
        Vec2(0.0f, 0.0f) {}

    Vec2 operator*(float s) const;

    float operator*(const Vec2 &) const;

    Vec2 operator/(float) const;

    Vec2 operator-(const Vec2 &rhs) const;

    Vec2 operator+(const Vec2 &rhs) const;

    Vec2 &operator+=(const Vec2 &rhs);

    Vec2 &operator*=(float k);

    bool operator==(const Vec2 &rhs) const;

    bool operator!=(const Vec2 &rhs) const;

    Vec2 normalized() const;

    float dot(const Vec2 &p) const;

    float length() const;

    float lengthSquared() const;

    float distanceToSqr(const Vec2 &p) const;

    void negate();

    Vec2 operator-() const;
};
