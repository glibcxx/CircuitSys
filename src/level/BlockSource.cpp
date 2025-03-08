#include "BlockSource.h"

#include "block/Block.h"

std::unordered_map<BlockPos, SharedPtr<BlockLegacy>> dummyBlockLegacys;
std::unordered_map<BlockPos, Block>                  dummyBlocks;

const Block &BlockSource::getBlock(const BlockPos &pos) const
{
    auto block = dummyBlocks.find(pos);
    if (block == dummyBlocks.end())
    {
        auto legacy = dummyBlockLegacys.emplace(pos, SharedPtr<BlockLegacy>::make(pos.toString(), (int)pos.hashCode()));
        dummyBlocks.emplace(pos, legacy.first->second.createWeakPtr());
        block = dummyBlocks.find(pos);
    }
    return block->second;
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

const BlockPos &BlockSource::getPlaceChunkPos() const
{
    return this->mPlaceChunkPos;
}