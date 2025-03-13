#pragma once

#include "BlockPos.h"

class Actor;

class ChunkPos
{
public:
    static const ChunkPos INVALID;
    static const ChunkPos MIN;
    static const ChunkPos MAX;

    ChunkPos max(const ChunkPos &a, const ChunkPos &b);

    ChunkPos min(const ChunkPos &a, const ChunkPos &b);

    union
    {
        int64_t packed;
        struct
        {
            int x;
            int z;
        };
    };

    ChunkPos() :
        x(0), z(0) {}

    ChunkPos(int x_, int z_) :
        x(x_), z(z_) {}

    ChunkPos(const BlockPos &pos) :
        x(pos.x >> 4), z(pos.z >> 4) {}

    ChunkPos(const Vec3 &pos) :
        ChunkPos(BlockPos{pos}) {}

    size_t hashCode() const;

    bool operator==(const ChunkPos &rhs) const;

    bool operator!=(const ChunkPos &rhs) const;

    ChunkPos &operator=(const ChunkPos &rhs);

    ChunkPos operator+(const ChunkPos &rhs) const;

    ChunkPos operator-(const ChunkPos &rhs) const;

    ChunkPos operator+(int t) const;

    ChunkPos operator-(int t) const;

    int distanceToSqr(const ChunkPos &pos) const;

    float distanceToSqr(const Actor &e) const;

    ChunkPos north() const;

    ChunkPos south() const;

    ChunkPos east() const;

    ChunkPos west() const;

    bool isNeighbor(const ChunkPos &) const;

    int getMiddleBlockX() const;

    int getMiddleBlockZ() const;

    BlockPos getMiddleBlockPosition(int y) const;
};

inline const ChunkPos ChunkPos::INVALID = {(int)0x80000000, (int)0x80000000};
inline const ChunkPos ChunkPos::MIN = {(int)0x80000000, (int)0x80000000};
inline const ChunkPos ChunkPos::MAX = {__INT32_MAX__, __INT32_MAX__};

namespace std
{
    template <>
    struct hash<ChunkPos>
    {
        std::size_t operator()(const ChunkPos &_Keyval) const
        {
            return _Keyval.hashCode();
        }
    };
}; // namespace std
