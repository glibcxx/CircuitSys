#pragma once

#include "Facing.h"
#include "deps/Math.h"

class Vec3;

enum class Rotation : uint8_t;
enum class Mirror : uint8_t;
class ChunkPos;

class BlockPos
{
public:
    static const BlockPos MIN;
    static const BlockPos MAX;
    static const BlockPos ONE;
    static const BlockPos ZERO;

    static BlockPos max(const BlockPos &a, const BlockPos &b);

    static BlockPos min(const BlockPos &a, const BlockPos &b);

    static bool boundsContain(const BlockPos &, const BlockPos &, const BlockPos &);

    int x;
    int y;
    int z;

    BlockPos(int a) :
        BlockPos(a, a, a) {}

    BlockPos() :
        BlockPos(0) {}

    BlockPos(int _x, int _y, int _z) :
        x(_x), y(_y), z(_z) {}

    BlockPos(const BlockPos &rhs) = default;

    BlockPos(const Vec3 &v);

    BlockPos(const ChunkPos &cp, int y);

    BlockPos(float x, float y, float z) :
        x(x), y(x), z(x) {}

    BlockPos(double x, double y, double z) :
        x(x), y(x), z(x) {}

    void set(int _x, int _y, int _z);

    void set(const BlockPos &pos);

    BlockPos &operator=(const BlockPos &rhs);

    BlockPos operator*(int s) const;

    const BlockPos &operator*=(int);

    BlockPos operator/(int s) const;

    BlockPos operator+(const BlockPos &rhs) const;

    BlockPos operator-(const BlockPos &rhs) const;

    BlockPos operator+(int s) const;

    BlockPos operator-(int s) const;

    const BlockPos &operator+=(const BlockPos &rhs);

    const BlockPos &operator-=(const BlockPos &rhs);

    BlockPos operator-() const;

    const int &operator[](int idx) const;

    int &operator[](int idx);

    //
    // operator glm::tvec3<int>() const;

    BlockPos offset(int _x, int _y, int _z) const;

    BlockPos above() const;

    BlockPos above(int steps) const;

    BlockPos below() const;

    BlockPos below(int steps) const;

    BlockPos north() const;

    BlockPos north(int steps) const;

    BlockPos south() const;

    BlockPos south(int steps) const;

    BlockPos west() const;

    BlockPos west(int steps) const;

    BlockPos east() const;

    BlockPos east(int steps) const;

    BlockPos neighbor(FacingID direction) const;

    Vec3 center() const;

    void move(int, int, int);

    void move(const BlockPos &);

    void moveX(int steps);

    void moveY(int steps);

    void moveZ(int steps);

    void moveUp(int steps);

    void moveUp();

    void moveDown(int steps);

    void moveDown();

    void moveEast(int steps);

    void moveEast();

    void moveWest(int steps);

    void moveWest();

    void moveNorth(int steps);

    void moveNorth();

    void moveSouth(int steps);

    void moveSouth();

    void clamp(const BlockPos &, const BlockPos &);

    std::string toString() const;

    uint64_t distSqr(const BlockPos &pos) const;

    uint64_t lengthSqr() const;

    float distSqrToCenter(float x_, float y_, float z_) const;

    bool operator==(const BlockPos &rhs) const;

    bool operator!=(const BlockPos &rhs) const;

    size_t hashCode() const;

    BlockPos toRenderChunkPosition() const;

    BlockPos relative(FacingID facing, int steps) const;

    BlockPos transform(Rotation rotation, Mirror mirror, const Vec3 &pivot) const;

    int32_t randomSeed() const;

    float randomFloat() const;
};

class BlockPosIterator
{
public:
    BlockPosIterator(const BlockPos &a, const BlockPos &b) :
        mMinCorner(BlockPos::min(a, b)),
        mMaxCorner(BlockPos::max(a, b)),
        mCurrentPos(this->mMinCorner),
        mDone(false) {}

    bool operator!=(const BlockPosIterator &other) const;

    BlockPosIterator &operator++();

    const BlockPos &operator*();

    BlockPosIterator begin() const;

    BlockPosIterator end() const;

private:
    const BlockPos mMinCorner;
    const BlockPos mMaxCorner;
    BlockPos       mCurrentPos;
    bool           mDone;
};

namespace std
{
    template <>
    struct hash<BlockPos>
    {
        std::size_t operator()(const BlockPos &_Keyval) const
        {
            return _Keyval.hashCode();
        }
    };
}; // namespace std

Vec3 operator+(const BlockPos &pos, const Vec3 &rhs);