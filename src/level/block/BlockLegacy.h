#pragma once

#include <cstdint>
#include <functional>
#include "level/BlockPos.h"

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
};

enum class BlockSupportType : int
{
    Center = 0,
    Edge = 1,
    Any = 2,
};

class BlockLegacy
{
public:
    friend class Block;

    static inline const float SIZE_OFFSET = 0.0001f;

    static const int UPDATE_NEIGHBORS = 1;
    static const int UPDATE_CLIENTS = 2;
    static const int UPDATE_INVISIBLE = 4;
    static const int UPDATE_ITEM_DATA = 16;
    static const int UPDATE_NONE = 4;
    static const int UPDATE_ALL = 3;
    static const int TILE_NUM_SHIFT = 12;
    static const int NUM_LEGACY_BLOCK_TYPES = 512;

    BlockLegacy(const std::string &nameId, int id) {}

    bool operator==(const BlockLegacy &rhs) const { return this == &rhs; }

    bool operator!=(const BlockLegacy &rhs) const { return this != &rhs; }

    void createBlockPermutations(uint32_t latestUpdaterVersion);

    ~BlockLegacy() = default;

    BlockLegacy(const BlockLegacy &other) = delete;

    BlockLegacy(BlockLegacy &&other) = delete;

    BlockLegacy &operator=(const BlockLegacy &other) = delete;

    BlockLegacy &operator=(BlockLegacy &&other) = delete;

    virtual void onRedstoneUpdate(BlockSource &region, const BlockPos &pos, int strength, bool isFirstTime) const {}

    virtual BlockProperty getRedstoneProperty(BlockSource &region, const BlockPos &pos) const { return this->mProperties; }

    BlockProperty mProperties{BlockProperty::CubeShaped};
    bool          mSolid = false;
};
