#include "ChunkPos.h"

ChunkPos ChunkPos::max(const ChunkPos &a, const ChunkPos &b)
{
    return {std::max(a.x, b.x), std::max(a.z, b.z)};
}

ChunkPos ChunkPos::min(const ChunkPos &a, const ChunkPos &b)
{
    return {std::min(a.x, b.x), std::min(a.z, b.z)};
}

size_t ChunkPos::hashCode() const
{
    return mce::Math::hash2(this->x, &this->z);
}

bool ChunkPos::operator==(const ChunkPos &rhs) const
{
    return this->packed == rhs.packed;
}

bool ChunkPos::operator!=(const ChunkPos &rhs) const
{
    return this->packed != rhs.packed;
}

ChunkPos &ChunkPos::operator=(const ChunkPos &rhs)
{
    this->packed = rhs.packed;
    return *this;
}

ChunkPos ChunkPos::operator+(const ChunkPos &rhs) const
{
    return {this->x + rhs.x, this->z + rhs.z};
}

ChunkPos ChunkPos::operator-(const ChunkPos &rhs) const
{
    return {this->x - rhs.x, this->z - rhs.z};
}

ChunkPos ChunkPos::operator+(int t) const
{
    return {this->x + t, this->z + t};
}

ChunkPos ChunkPos::operator-(int t) const
{
    return {this->x - t, this->z - t};
}

int ChunkPos::distanceToSqr(const ChunkPos &pos) const
{
    int a = pos.x - this->x;
    int b = pos.z - this->z;
    return a * a + b * b;
}

ChunkPos ChunkPos::north() const
{
    return {this->x, this->z - 1};
}

ChunkPos ChunkPos::south() const
{
    return {this->x, this->z + 1};
}

ChunkPos ChunkPos::east() const
{
    return {this->x + 1, this->z};
}

ChunkPos ChunkPos::west() const
{
    return {this->x - 1, this->z};
}

bool ChunkPos::isNeighbor(const ChunkPos &pos) const
{
    return std::abs(this->x - pos.x) <= 1 && std::abs(this->z - pos.z) <= 1;
}

int ChunkPos::getMiddleBlockX() const
{
    return 16 * this->x + 8;
}

int ChunkPos::getMiddleBlockZ() const
{
    return 16 * this->z + 8;
}

BlockPos ChunkPos::getMiddleBlockPosition(int y) const
{
    return {this->getMiddleBlockX(), y, this->getMiddleBlockZ()};
}
