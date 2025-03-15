#pragma once

#include <string>
#include <vector>
#include <functional>
#include "BlockLegacy.h"

class Block
{
public:
    Block(const WeakPtr<BlockLegacy> &oldBlock) :
        mLegacyBlock(oldBlock) {}

    bool isSolid() const { return this->mLegacyBlock->mSolid; }

    bool operator==(const Block &rhs) const { return *this->mLegacyBlock == *rhs.mLegacyBlock; }

    bool operator!=(const Block &rhs) const { return *this->mLegacyBlock != *rhs.mLegacyBlock; }

    void onRedstoneUpdate(BlockSource &region, const BlockPos &pos, int strength, bool isFirstTime) const { this->mLegacyBlock->onRedstoneUpdate(region, pos, strength, isFirstTime); }
    void tick(BlockSource &region, const BlockPos &pos, std::mt19937 &random) const { this->mLegacyBlock->tick(region, pos, random); }

    BlockProperty getRedstoneProperty(BlockSource &region, const BlockPos &pos) const { return this->mLegacyBlock->getRedstoneProperty(region, pos); }

    const BlockLegacy &getLegacyBlock() const { return *this->mLegacyBlock; }

private:
    WeakPtr<BlockLegacy> mLegacyBlock;
};
