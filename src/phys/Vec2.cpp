#include "Vec2.h"

#include <cmath>

inline const Vec2 Vec2::LOWEST = {std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};
inline const Vec2 Vec2::MAX = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
inline const Vec2 Vec2::MIN = {std::numeric_limits<float>::min(), std::numeric_limits<float>::min()};
inline const Vec2 Vec2::NEG_UNIT_X = {-1.0f, 0.0f};
inline const Vec2 Vec2::NEG_UNIT_Y = {0.0f, -1.0f};
inline const Vec2 Vec2::ONE = {1.0f, 1.0f};
inline const Vec2 Vec2::UNIT_X = {1.0f, 0.0f};
inline const Vec2 Vec2::UNIT_Y = {0.0f, 1.0f};
inline const Vec2 Vec2::ZERO = {};

Vec2 Vec2::bilinear(const Vec2 &, const Vec2 &, const Vec2 &, const Vec2 &, float, float)
{
    return Vec2();
}

Vec2 Vec2::bilinear(std::array<Vec2, 4ul> const &, float, float)
{
    return Vec2();
}

Vec2 Vec2::operator*(float s) const
{
    return Vec2(this->x * s, this->y * s);
}

float Vec2::operator*(const Vec2 &p) const
{
    return this->dot(p);
}

Vec2 Vec2::operator-(const Vec2 &rhs) const
{
    return Vec2(this->x - rhs.x, this->y - rhs.y);
}

Vec2 Vec2::operator+(const Vec2 &rhs) const
{
    return Vec2(this->x + rhs.x, this->y + rhs.y);
}

Vec2 &Vec2::operator+=(const Vec2 &rhs)
{
    this->x = rhs.x + this->x;
    this->y = rhs.y + this->y;
    return *this;
}

Vec2 &Vec2::operator*=(float k)
{
    this->x = k * this->x;
    this->y = k * this->y;
    return *this;
}

bool Vec2::operator==(const Vec2 &rhs) const
{
    return this->x == rhs.x && this->y == rhs.y;
}

bool Vec2::operator!=(const Vec2 &rhs) const
{
    return this->x != rhs.x || this->y != rhs.y;
}

Vec2 Vec2::normalized() const
{
    float dist = std::sqrt(this->x * this->x + this->y * this->y);
    return dist >= 0.0001f ? Vec2(this->x / dist, this->y / dist) : Vec2::ZERO;
}

float Vec2::dot(const Vec2 &p) const
{
    return this->x * p.x + this->y * p.y;
}

float Vec2::length() const
{
    return std::sqrt(this->x * this->x + this->y * this->y);
}

float Vec2::lengthSquared() const
{
    return this->x * this->x + this->y * this->y;
}

float Vec2::distanceToSqr(const Vec2 &p) const
{
    float xd = p.x - this->x;
    float yd = p.y - this->y;
    return xd * xd + yd * yd;
}

Vec2 Vec2::operator-() const
{
    return Vec2(-this->x, -this->y);
}
