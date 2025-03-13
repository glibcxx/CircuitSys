#pragma once

#include <string>
#include <vector>
#include <functional>
#include "BlockLegacy.h"
#include "SharedPtr.h"

namespace BedrockBlockTypes
{
    inline SharedPtr<BlockLegacy> mAir_ = SharedPtr<BlockLegacy>::make("air", 0);
    inline WeakPtr<BlockLegacy>   mAir = mAir_.createWeakPtr();

}; // namespace BedrockBlockTypes

class Block
{
public:
    Block(const WeakPtr<BlockLegacy> &oldBlock) :
        mLegacyBlock(oldBlock) {}

    bool isSolid() const { return this->mLegacyBlock->mSolid; }

    void onRedstoneUpdate(BlockSource &region, const BlockPos &pos, int strength, bool isFirstTime) const { this->mLegacyBlock->onRedstoneUpdate(region, pos, strength, isFirstTime); }

    BlockProperty getRedstoneProperty(BlockSource &region, const BlockPos &pos) const { return this->mLegacyBlock->getRedstoneProperty(region, pos); }

    const BlockLegacy &getLegacyBlock() const { return *this->mLegacyBlock; }

private:
    WeakPtr<BlockLegacy> mLegacyBlock;
};
