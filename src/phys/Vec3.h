#pragma once

#include "level/BlockPos.h"
#include "Pos.h"
#include "Vec2.h"

class Vec3
{
public:
    static const Vec3 ZERO, ONE, TWO, HALF, UNIT_X, NEG_UNIT_X, UNIT_Y, NEG_UNIT_Y, UNIT_Z, NEG_UNIT_Z, MAX, MIN;

    static Vec3 lerp(const Vec3 &a, const Vec3 &b, float s);

    Vec3 lerpComponent(const Vec3 &a, const Vec3 &b, const Vec3 &s);

    static Vec3 min(const Vec3 &a, const Vec3 &b);

    static Vec3 max(const Vec3 &a, const Vec3 &b);

    Vec3 mul(const Vec3 &a, const Vec3 &b);

    Vec3(float s) :
        x(s), y(s), z(s) {}

    Vec3() :
        Vec3(0.0f) {}

    Vec3(const BlockPos &pos) :
        x(pos.x), y(pos.y), z(pos.z) {}

    Vec3(const Pos &pos) :
        x(pos.x), y(pos.y), z(pos.z) {}

    Vec3(float x_, float y_, float z_) :
        x(x_), y(y_), z(z_) {}

    Vec3 *set(float x_, float y_, float z_);

    Vec3 operator+(const Vec3 &rhs) const;

    Vec3 operator+(float f) const;

    Vec3 &operator=(const Vec3 &rhs);

    Vec3 &operator+=(const Vec3 &rhs);

    Vec3 operator-(const Vec3 &rhs) const;

    Vec3 operator-(float f) const;

    Vec3 &operator-=(const Vec3 &rhs);

    Vec3 operator*(float k) const;

    Vec3 operator/(float k) const;

    float operator*(const Vec3 &v) const;

    Vec3 &operator*=(float k);

    bool operator==(const Vec3 &rhs) const;

    bool operator!=(const Vec3 &rhs) const;

    float const &operator[](int k) const;

    float &operator[](int k);

    operator bool() const;

    Vec3 normalized() const;

    float dot(const Vec3 &p) const;

    Vec3 operator^(const Vec3 &p) const;

    Vec3 cross(const Vec3 &p) const;

    Vec3 add(float x_, float y_, float z_) const;

    Vec3 sub(float x_, float y_, float z_) const;

    void negate();

    Vec3 negated() const;

    Vec3 operator-() const;

    float distanceTo(const Vec3 &p) const;

    float distanceToSqr(const Vec3 &p) const;

    float distanceToSqr(float a, float b, float c) const;

    float length() const;

    float lengthSquared() const;

    float lengthXZ() const;

    float lengthXZSquared() const;

    Vec3 projectOntoLine(const Vec3 &p0, const Vec3 &p1) const;

    float distanceToLineSquared(const Vec3 &p0, const Vec3 &p1) const;

    float distanceToLine(const Vec3 &p0, const Vec3 &p1) const;

    float distanceToLineSegmentSquared(const Vec3 &p0, const Vec3 &p1) const;

    float distanceToLineSegment(const Vec3 &p0, const Vec3 &p1) const;

    float minComponent() const;

    float maxComponent() const;

    bool clipX(const Vec3 &a, float, Vec3 &b) const;

    bool clipY(const Vec3 &a, float, Vec3 &b) const;

    bool clipZ(const Vec3 &a, float, Vec3 &b) const;

    static Vec3 clamp(const Vec3 &v, const Vec3 &clipMin, const Vec3 &clipMax);

    static bool clampAlongNormal(const Vec3 &input, const Vec3 &delta, const Vec3 &clipMin, const Vec3 &clipMax, Vec3 &result);

    std::string toString() const;

    std::string toJsonString() const;

    void yRot(float rads);

    Vec3 xz() const;

    Vec3 abs() const;

    Vec3 floor(float offset) const;

    Vec3 ceil() const;

    Vec3 round() const;

    bool isNan() const;

    static Vec2 rotationFromDirection(const Vec3 &dir);

    float x, y, z;
};
