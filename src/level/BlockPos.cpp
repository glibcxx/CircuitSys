#include "BlockPos.h"

#include <random>
#include <string>
#include <sstream>
#include "phys/Vec3.h"
#include "util/MirrorEnum.h"
#include "util/RotationEnum.h"
#include "level/ChunkPos.h"

const BlockPos BlockPos::MIN = {INT_MIN, INT_MIN, INT_MIN};
const BlockPos BlockPos::MAX = {INT_MAX, INT_MAX, INT_MAX};
const BlockPos BlockPos::ONE = {1, 1, 1};
const BlockPos BlockPos::ZERO = {0, 0, 0};

BlockPos BlockPos::max(const BlockPos &a, const BlockPos &b)
{
    return {std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z)};
}

BlockPos BlockPos::min(const BlockPos &a, const BlockPos &b)
{
    return {std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z)};
}

bool BlockPos::boundsContain(const BlockPos &min, const BlockPos &max, const BlockPos &pos)
{
    return pos.x >= min.x && pos.x <= max.x && pos.y >= min.y && pos.y <= max.y && pos.z >= min.z && pos.z <= max.z;
}

BlockPos::BlockPos(const Vec3 &v) :
    x(v.x), y(v.y), z(v.z) {}

BlockPos::BlockPos(const ChunkPos &cp, int y) :
    x(cp.x), y(y), z(cp.z) {}

void BlockPos::set(int _x, int _y, int _z)
{
    this->x = _x;
    this->y = _y;
    this->z = _z;
}

void BlockPos::set(const BlockPos &pos)
{
    this->x = pos.x;
    this->y = pos.y;
    this->z = pos.z;
}

BlockPos &BlockPos::operator=(const BlockPos &rhs)
{
    this->x = rhs.x;
    this->y = rhs.y;
    this->z = rhs.z;
    return *this;
}

float BlockPos::distSqrToCenter(float x_, float y_, float z_) const
{
    float x_diff = this->x + 0.5f - x_;
    float y_diff = this->y + 0.5f - y_;
    float z_diff = this->z + 0.5f - z_;
    return x_diff * x_diff + y_diff * y_diff + z_diff * z_diff;
}

uint64_t BlockPos::distSqr(const BlockPos &pos) const
{
    int64_t x_diff = this->x - pos.x;
    int64_t y_diff = this->y - pos.y;
    int64_t z_diff = this->z - pos.z;
    return x_diff * x_diff + y_diff * y_diff + z_diff * z_diff;
}

uint64_t BlockPos::lengthSqr() const
{
    return (uint64_t)this->x * this->x + (uint64_t)this->y * this->y + (uint64_t)this->z * this->z;
}

bool BlockPos::operator==(const BlockPos &v) const
{
    return x == v.x && y == v.y && z == v.z;
}

const BlockPos &BlockPos::operator+=(const BlockPos &o)
{
    this->x += o.x;
    this->y += o.y;
    this->z += o.z;
    return *this;
}

const BlockPos &BlockPos::operator-=(const BlockPos &o)
{
    this->x -= o.x;
    this->y -= o.y;
    this->z -= o.z;
    return *this;
}

BlockPos BlockPos::operator-(const BlockPos &rhs) const
{
    return {this->x - rhs.x, this->y - rhs.y, this->z - rhs.z};
}

BlockPos BlockPos::operator+(int s) const
{
    return {this->x + s, this->y + s, this->z + s};
}

BlockPos BlockPos::operator-(int s) const
{
    return {this->x - s, this->y - s, this->z - s};
}

BlockPos BlockPos::operator-() const
{
    return {-this->x, -this->y, -this->z};
}

bool BlockPos::operator!=(const BlockPos &v) const
{
    return x != v.x || y != v.y || z != v.z;
}

size_t BlockPos::hashCode() const
{
    return mce::Math::hash3(this->x, this->y, this->z);
}

BlockPos BlockPos::relative(FacingID facing, int steps) const
{
    return {steps * Facing::getStepX(facing) + this->x,
            steps * Facing::getStepY(facing) + this->y,
            steps * Facing::getStepZ(facing) + this->z};
}

BlockPos BlockPos::transform(Rotation rotation, Mirror mirror, const Vec3 &pivot) const
{
    BlockPos pos{*this};
    switch (mirror) // NOLINT
    {
    case Mirror::X:
        pos.z = 2.0f * pivot.z - pos.z;
        break;
    case Mirror::Z:
        pos.x = 2.0f * pivot.x - pos.x;
        break;
    case Mirror::XZ:
        pos.x = 2.0f * pivot.x - pos.x;
        pos.z = 2.0f * pivot.z - pos.z;
        break;
    }
    switch (rotation)
    {
    case Rotation::Rotate90:
        return {(int)(2.0f * pivot.z - pos.z), pos.y, pos.x};
        break;
    case Rotation::Rotate180:
        return {(int)(2.0f * pivot.x - pos.x), pos.y, (int)(2.0f * pivot.z - pos.z)};
        break;
    case Rotation::Rotate270:
        return {pos.z, pos.y, (int)(2.0f * pivot.x - pos.x)};
        break;
    default:
        return pos;
        break;
    }
}

int32_t BlockPos::randomSeed() const
{
    int seed = this->y ^ (116129781 * this->z) ^ (3129871 * this->x);
    return 11 * seed + 42317861 * seed * seed;
}

float BlockPos::randomFloat() const
{
    return (this->randomSeed() >> 16) / 65535.0f;
}

std::string BlockPos::toString() const
{
    std::stringstream ss;
    ss << "Pos(" << this->x << "," << this->y << "," << this->z << ")";
    return ss.str();
}

BlockPos BlockPos::offset(int _x, int _y, int _z) const
{
    return {this->x + _x, this->y + _y, this->z + _z};
}

BlockPos BlockPos::above(int steps) const
{
    return {this->x, this->y + steps, this->z};
}

BlockPos BlockPos::above() const
{
    return {this->x, this->y + 1, this->z};
}

BlockPos BlockPos::below(int steps) const
{
    return {this->x, this->y - steps, this->z};
}

BlockPos BlockPos::below() const
{
    return {this->x, this->y - 1, this->z};
}

BlockPos BlockPos::north(int steps) const
{
    return {this->x, this->y, this->z - steps};
}

BlockPos BlockPos::north() const
{
    return {this->x, this->y, this->z - 1};
}

BlockPos BlockPos::south(int steps) const
{
    return {this->x, this->y, this->z + steps};
}

BlockPos BlockPos::south() const
{
    return {this->x, this->y, this->z + 1};
}

BlockPos BlockPos::west(int steps) const
{
    return {this->x - steps, this->y, this->z};
}

BlockPos BlockPos::west() const
{
    return {this->x - 1, this->y, this->z};
}

BlockPos BlockPos::east(int steps) const
{
    return {this->x + steps, this->y, this->z};
}

BlockPos BlockPos::east() const
{
    return {this->x + 1, this->y, this->z};
}

BlockPos BlockPos::neighbor(FacingID direction) const
{
    return this->relative(direction, 1);
}

Vec3 BlockPos::center() const
{
    return {this->x + 0.5f, this->y + 0.5f, this->z + 0.5f};
}

BlockPos BlockPos::operator*(int s) const
{
    return {this->x * s, this->y * s, this->z * s};
}

const BlockPos &BlockPos::operator*=(int s)
{
    this->x *= s;
    this->y *= s;
    this->z *= s;
    return *this;
}

BlockPos BlockPos::operator/(int s) const
{
    return {this->x / s, this->y / s, this->z / s};
}

BlockPos BlockPos::operator+(const BlockPos &pos) const
{
    return {x + pos.x, y + pos.y, z + pos.z};
}

void BlockPos::moveUp()
{
    ++this->y;
}

void BlockPos::moveDown()
{
    --this->y;
}

void BlockPos::moveY(int steps)
{
    this->y += steps;
}

bool BlockPosIterator::operator!=(const BlockPosIterator &other) const
{
    return this->mMinCorner != other.mMinCorner
        || this->mMaxCorner != other.mMaxCorner
        || this->mCurrentPos != other.mCurrentPos
        || this->mDone != other.mDone;
}

BlockPosIterator &BlockPosIterator::operator++()
{
    if (this->mCurrentPos.z >= this->mMaxCorner.z)
    {
        if (this->mCurrentPos.y >= this->mMaxCorner.y)
        {
            if (this->mCurrentPos.x >= this->mMaxCorner.x)
            {
                this->mDone = 1;
            }
            else
            {
                this->mCurrentPos.z = this->mMinCorner.z;
                this->mCurrentPos.y = this->mMinCorner.y;
                ++this->mCurrentPos.x;
            }
        }
        else
        {
            this->mCurrentPos.z = this->mMinCorner.z;
            ++this->mCurrentPos.y;
        }
    }
    else
    {
        ++this->mCurrentPos.z;
    }
    return *this;
}

const BlockPos &BlockPosIterator::operator*()
{
    return this->mCurrentPos;
}

BlockPosIterator BlockPosIterator::begin() const
{
    return {this->mMinCorner, this->mMaxCorner};
}

BlockPosIterator BlockPosIterator::end() const
{
    BlockPosIterator retstr{this->mMinCorner, this->mMaxCorner};
    retstr.mCurrentPos = this->mMaxCorner;
    retstr.mDone = true;
    return retstr;
}

Vec3 operator+(const BlockPos &pos, const Vec3 &rhs)
{
    return {pos.x + rhs.x, pos.y + rhs.y, pos.z + rhs.z};
}
