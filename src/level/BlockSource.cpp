#include "BlockSource.h"

#include "block/Block.h"

const Block &BlockSource::getBlock(const BlockPos &pos) const
{
    auto iter = this->mBlockMap.find(pos);
    if (iter == this->mBlockMap.end())
        return *FakeBlocks::mAir;
    else
        return *iter->second;
}

LevelChunk *BlockSource::getChunkAt(const BlockPos &pos) const
{
    return nullptr; /* dummy */
}

bool BlockSource::hasChunksAt(const BlockPos &pos, int r) const
{
    return true; /* dummy */
}

bool BlockSource::areChunksFullyLoaded(const BlockPos &pos, int r)
{
    return true; /* dummy */
}

void BlockSource::setBlock(const BlockPos &pos, const Block &block)
{
    this->mBlockMap[pos] = &block;
}

const BlockPos &BlockSource::getPlaceChunkPos() const
{
    return this->mPlaceChunkPos;
}