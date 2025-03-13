#include "Vec3.h"

#include <cfloat>
#include <cmath>
#include <sstream>
#include "deps/Math.h"

const Vec3 Vec3::ZERO = {0};
const Vec3 Vec3::ONE = {1};
const Vec3 Vec3::TWO = {2};
const Vec3 Vec3::HALF = {0.5f};

const Vec3 Vec3::MAX = {FLT_MAX};
const Vec3 Vec3::MIN = {-FLT_MAX};

Vec3 *Vec3::set(float x_, float y_, float z_)
{
    this->x = x_;
    this->y = y_;
    this->z = z_;
    return this;
}

Vec3 Vec3::operator*(float k) const
{
    return {this->x * k, this->y * k, this->z * k};
}

Vec3 Vec3::operator/(float k) const
{
    return *this * (1.0f / k);
}

float Vec3::operator*(const Vec3 &v) const
{
    return this->dot(v);
}

Vec3 &Vec3::operator*=(float k)
{
    this->x = k * this->x;
    this->y = k * this->y;
    this->z = k * this->z;
    return *this;
}

Vec3 Vec3::operator+(const Vec3 &rhs) const
{
    return {this->x + rhs.x, this->y + rhs.y, this->z + rhs.z};
}

Vec3 Vec3::operator+(float f) const
{
    return {this->x + f, this->y + f, this->z + f};
}

Vec3 &Vec3::operator=(const Vec3 &rhs)
{
    this->x = rhs.x;
    this->y = rhs.y;
    this->z = rhs.z;
    return *this;
}

Vec3 Vec3::operator-(const Vec3 &rhs) const
{
    return {this->x - rhs.x, this->y - rhs.y, this->z - rhs.z};
}

Vec3 Vec3::operator-(float f) const
{
    return {this->x - f, this->y - f, this->z - f};
}

Vec3 &Vec3::operator-=(const Vec3 &rhs)
{
    this->x = this->x - rhs.x;
    this->y = this->y - rhs.y;
    this->z = this->z - rhs.z;
    return *this;
}

Vec3 &Vec3::operator+=(const Vec3 &rhs)
{
    this->x = rhs.x + this->x;
    this->y = rhs.y + this->y;
    this->z = rhs.z + this->z;
    return *this;
}
bool Vec3::operator==(const Vec3 &rhs) const
{
    return this->x == rhs.x && this->y == rhs.y && this->z == rhs.z;
}

bool Vec3::operator!=(const Vec3 &rhs) const
{
    return !(*this == rhs);
}

Vec3 Vec3::min(const Vec3 &a, const Vec3 &b)
{
    return {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z)};
}

Vec3 Vec3::max(const Vec3 &a, const Vec3 &b)
{
    return {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)};
}

Vec3 Vec3::mul(const Vec3 &a, const Vec3 &b)
{
    return {a.x * b.x, a.y * b.y, a.z * b.z};
}

const float &Vec3::operator[](int index) const
{
    switch (index)
    {
    case 0:
        return this->x;
    case 1:
        return this->y;
    case 2:
        return this->z;
    }
    return this->x;
}

float &Vec3::operator[](int index)
{
    switch (index)
    {
    case 0:
        return this->x;
    case 1:
        return this->y;
    case 2:
        return this->z;
    }
    return this->x;
}

Vec3::operator bool() const
{
    return this->x != 0.0f || this->y != 0.0f || this->z != 0.0f;
}

Vec3 Vec3::normalized() const
{
    float dist = std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
    return dist < 0.0001f ? Vec3::ZERO : Vec3{this->x / dist, this->y / dist, this->z / dist};
}

float Vec3::dot(const Vec3 &p) const
{
    return this->x * p.x + this->y * p.y + this->z * p.z;
}

Vec3 Vec3::operator^(const Vec3 &p) const
{
    return Vec3(this->y * p.z - this->z * p.y,
                this->z * p.x - this->x * p.z,
                this->x * p.y - this->y * p.x);
}

Vec3 Vec3::cross(const Vec3 &p) const
{
    return *this ^ p;
}

Vec3 Vec3::add(float x_, float y_, float z_) const
{
    return {this->x + x_, this->y + y_, this->z + z_};
}

Vec3 Vec3::sub(float x_, float y_, float z_) const
{
    return {this->x - x_, this->y - y_, this->z - z_};
}

Vec3 Vec3::negated() const
{
    return {-this->x, -this->y, -this->z};
}

float Vec3::lengthSquared() const
{
    return this->x * this->x + this->y * this->y + this->z * this->z;
}

float Vec3::lengthXZ() const
{
    return std::sqrt(this->x * this->x + this->z * this->z);
}

float Vec3::lengthXZSquared() const
{
    return this->x * this->x + this->z * this->z;
}

Vec3 Vec3::projectOntoLine(const Vec3 &p0, const Vec3 &p1) const
{
    Vec3 A0 = *this - p0;
    Vec3 ray = p1 - p0;
    return p0 + ray * (A0 * ray / ray * ray);
}

float Vec3::distanceToLineSquared(const Vec3 &p0, const Vec3 &p1) const
{
    Vec3 projection = this->projectOntoLine(p0, p1);
    return (projection - *this).lengthSquared();
}

float Vec3::distanceToLine(const Vec3 &p0, const Vec3 &p1) const
{
    Vec3 projection = this->projectOntoLine(p0, p1);
    return (projection - *this).length();
}

float Vec3::distanceToLineSegmentSquared(const Vec3 &p0, const Vec3 &p1) const
{
    Vec3  A0 = *this - p0;
    Vec3  ray = p1 - p0;
    float t = (A0 * ray) / (ray * ray);
    if (t < 0.0f)
        return this->distanceTo(p0);
    if (t > 1.0f)
        return this->distanceTo(p1);
    Vec3 projection = p0 + ray * t;
    return (projection - *this).lengthSquared();
}

float Vec3::distanceToLineSegment(const Vec3 &p0, const Vec3 &p1) const
{
    return std::sqrt(this->distanceToLineSegmentSquared(p0, p1));
}

float Vec3::minComponent() const
{
    return mce::Math::min(mce::Math::min(this->x, this->y), this->z);
}

float Vec3::maxComponent() const
{
    return mce::Math::max(mce::Math::max(this->x, this->y), this->z);
}

Vec3 Vec3::clamp(const Vec3 &v, const Vec3 &clipMin, const Vec3 &clipMax)
{
    float x_ = mce::Math::clamp(v.x, clipMin.x, clipMax.x);
    float y_ = mce::Math::clamp(v.y, clipMin.y, clipMax.y);
    float z_ = mce::Math::clamp(v.z, clipMin.z, clipMax.z);
    Vec3  result(x_, y_, z_);
    return result;
}

bool Vec3::clampAlongNormal(const Vec3 &input, const Vec3 &delta, const Vec3 &clipMin, const Vec3 &clipMax, Vec3 &result)
{
    Vec3 inputClamped = Vec3::clamp(input, clipMin, clipMax);
    if (inputClamped == input)
    {
        result = input;
        return true;
    }

    Vec3 error = inputClamped - input;
    if ((error.x == 0.0f || mce::Math::sign(delta.x) == mce::Math::sign(error.x)) && //
        (error.y == 0.0f || mce::Math::sign(delta.y) == mce::Math::sign(error.y)) && //
        (error.z == 0.0f || mce::Math::sign(delta.z) == mce::Math::sign(error.z)))
    {
        Vec3 errorOverDelta(error.x / delta.x, error.y / delta.y, error.z / delta.z);
        Vec3 possibleResult = input + delta * errorOverDelta.maxComponent();
        Vec3 clampedResultDelta = possibleResult - Vec3::clamp(possibleResult, clipMin, clipMax);
        if (clampedResultDelta.length() <= 0.01f)
        {
            result = possibleResult;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

std::string Vec3::toString() const
{
    std::stringstream ss;
    ss << "Vec3(" << this->x << "," << this->y << "," << this->z << ")";
    return ss.str();
}

std::string Vec3::toJsonString() const
{
    std::stringstream ss;
    ss << "[" << this->x << ", " << this->y << ", " << this->z << "]";
    return ss.str();
}

void Vec3::yRot(float rads)
{
    float cosv = std::cos(rads);
    float sinv = std::sin(rads);
    float xx = this->x * cosv + this->z * sinv;
    float yy = this->y;
    float zz = this->z * cosv - this->x * sinv;
    this->x = xx;
    this->y = yy;
    this->z = zz;
}

Vec3 Vec3::operator-() const
{
    return this->negated();
}

float Vec3::distanceTo(const Vec3 &p) const
{
    float xd = p.x - this->x;
    float yd = p.y - this->y;
    float zd = p.z - this->z;
    return std::sqrt(xd * xd + yd * yd + zd * zd);
}

float Vec3::distanceToSqr(const Vec3 &p) const
{
    float xd = p.x - this->x;
    float yd = p.y - this->y;
    float zd = p.z - this->z;
    return (xd * xd) + (yd * yd) + (zd * zd);
}

float Vec3::distanceToSqr(float a, float b, float c) const
{
    float xd = a - this->x;
    float yd = b - this->y;
    float zd = c - this->z;
    return (xd * xd) + (yd * yd) + (zd * zd);
}

float Vec3::length() const
{
    return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

Vec3 Vec3::lerp(const Vec3 &a, const Vec3 &b, float s)
{
    return {a.x + (b.x - a.x) * s, a.y + (b.y - a.y) * s, a.z + (b.z - a.z) * s};
}

Vec3 Vec3::lerpComponent(const Vec3 &a, const Vec3 &b, const Vec3 &s)
{
    return {a.x + (b.x - a.x) * s.x, a.y + (b.y - a.y) * s.y, a.z + (b.z - a.z) * s.z};
}

bool Vec3::clipX(const Vec3 &b, float xt, Vec3 &result) const
{
    float xd = b.x - this->x;
    if ((xd * xd) < 0.0000001f)
        return 0;
    float d = (xt - this->x) / xd;
    if (d < 0.0f || d > 1.0f)
        return 0;
    result.set(this->x + xd * d, this->y + (b.y - this->y) * d, this->z + (b.z - this->z) * d);
    return true;
}

bool Vec3::clipY(const Vec3 &b, float yt, Vec3 &result) const
{
    float yd = b.y - this->y;
    if ((yd * yd) < 0.0000001f)
        return 0;
    float d = (yt - this->y) / yd;
    if (d < 0.0f || d > 1.0f)
        return 0;
    result.set(this->x + (b.x - this->x) * d, this->y + (yd * d), this->z + (b.z - this->z) * d);
    return true;
}

bool Vec3::clipZ(const Vec3 &b, float zt, Vec3 &result) const
{
    float zd = b.z - this->z;
    if ((zd * zd) < 0.0000001f)
        return 0;
    float d = (zt - this->z) / zd;
    if (d < 0.0f || d > 1.0f)
        return 0;
    result.set(this->x + (b.x - this->x) * d, this->y + (b.y - this->y) * d, this->z + (zd * d));
    return true;
}

Vec3 Vec3::xz() const
{
    return {this->x, 0.0f, this->z};
}

Vec3 Vec3::abs() const
{
    return {std::abs(this->x), std::abs(this->y), std::abs(this->z)};
}

Vec3 Vec3::floor(float offset) const
{
    return {std::floor(this->x + offset), std::floor(this->y + offset), std::floor(this->z + offset)};
}

Vec3 Vec3::ceil() const
{
    return {std::ceil(this->x), std::ceil(this->y), std::ceil(this->z)};
}

Vec3 Vec3::round() const
{
    return Vec3(mce::Math::round(this->x), mce::Math::round(this->y), mce::Math::round(this->z));
}

bool Vec3::isNan() const
{
    return mce::Math::isNan(this->x) || mce::Math::isNan(this->y) || mce::Math::isNan(this->z);
}

Vec2 Vec3::rotationFromDirection(const Vec3 &dir)
{
    float sd = std::sqrt(dir.x * dir.x + dir.z * dir.z);
    float xRotD = std::atan2(dir.y, sd) * 57.295776f;
    float yRotD = std::atan2(dir.z, dir.x) * 57.295776f + -90.0f;
    return Vec2(xRotD, yRotD);
}
