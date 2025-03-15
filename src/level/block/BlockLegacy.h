#pragma once

#include <string>
#include <functional>
#include <random>
#include "level/BlockPos.h"
#include "FakeBlocks.h"
#include "util/EnumCast.h"

class BlockSource;
class Block;

enum class BlockProperty : int64_t
{
    None = 0x0,
    Stair = 0x1,
    HalfSlab = 0x2,
    Hopper = 0x4,
    TopSnow = 0x8,
    FenceGate = 0x10,
    Leaf = 0x20,
    ThinConnects2D = 0x40,
    Connects2D = 0x80,
    Carpet = 0x100,
    Button = 0x200,
    Door = 0x400,
    Portal = 0x800,
    Heavy = 0x1000,
    Snow = 0x2000,
    Trap = 0x4000,
    Sign = 0x8000,
    Walkable = 0x10000,
    PressurePlate = 0x20000,
    PistonBlockGrabber = 0x40000,
    TopSolidBlocking = 0x80000,
    SolidBlocking = 0x100000,
    CubeShaped = 0x200000,
    Power_NO = 0x400000,
    Power_BlockDown = 0x800000,
    Immovable = 0x1000000,
    BreakOnPush = 0x2000000,
    Piston = 0x4000000,
    InfiniBurn = 0x8000000,
    RequiresWorldBuilder = 0x10000000,
    CausesDamage = 0x20000000,
    BreaksWhenFallenOnByHeavy = 0x40000000,
    OnlyPistonPush = 0x80000000,
    Liquid = 0x100000000,
    CanBeBuiltOver = 0x200000000,
    SnowRecoverable = 0x400000000,
    Scaffolding = 0x800000000,
    CanSupportCenterHangingBlock = 0x1000000000,
    BreaksWhenHitByArrow = 0x2000000000,
    Unwalkable = 0x4000000000,
    Impenetrable = 0x8000000000,
    Hollow = 0x10000000000,
    OperatorBlock = 0x20000000000,
    SupportedByFlowerPot = 0x40000000000,
    PreventsJumping = 0x80000000000,
    ContainsHoney = 0x100000000000,
    Slime = 0x200000000000,
    SculkReplaceable = 0x400000000000,
    Climbable = 0x800000000000,
    CanHaltWhenClimbing = 0x1000000000000
};

inline BlockProperty operator&(BlockProperty lhs, BlockProperty b) { return BlockProperty{enum_cast(lhs) & enum_cast(b)}; }

inline BlockProperty operator|(BlockProperty lhs, BlockProperty b) { return BlockProperty{enum_cast(lhs) | enum_cast(b)}; }

inline BlockProperty &operator|=(BlockProperty &lhs, BlockProperty rhs)
{
    lhs = BlockProperty{enum_cast(lhs) | enum_cast(rhs)};
    return lhs;
}

class BlockLegacy
{
public:
    friend class Block;

    BlockLegacy(const std::string &nameId, int id) :
        mNameId(nameId), mId(id) {}

    bool operator==(const BlockLegacy &rhs) const { return this->mId == rhs.mId; }

    bool operator!=(const BlockLegacy &rhs) const { return this->mId != rhs.mId; }

    ~BlockLegacy() = default;

    BlockLegacy(const BlockLegacy &other) = delete;

    BlockLegacy(BlockLegacy &&other) = delete;

    BlockLegacy &operator=(const BlockLegacy &other) = delete;

    BlockLegacy &operator=(BlockLegacy &&other) = delete;

    virtual void onRedstoneUpdate(BlockSource &region, const BlockPos &pos, int strength, bool isFirstTime) const {}

    virtual void tick(BlockSource &region, const BlockPos &pos, std::mt19937 &random) {}

    virtual BlockProperty getRedstoneProperty(BlockSource &region, const BlockPos &pos) const { return this->mProperties; }

    const std::string mNameId;
    const int         mId;
    BlockProperty     mProperties{BlockProperty::CubeShaped};
    bool              mSolid = false;
};
