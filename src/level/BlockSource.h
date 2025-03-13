#pragma once

#include "ChunkPos.h"
#include "chunk/LevelChunk.h"

class Block;

class BlockSource
{
public:
    LevelChunk *getChunkAt(const BlockPos &pos) const;

    bool hasChunksAt(const BlockPos &pos, int r) const;

    bool areChunksFullyLoaded(const BlockPos &pos, int r);

    const Block &getBlock(const BlockPos &pos) const;

    const BlockPos &getPlaceChunkPos() const;

protected:
    BlockPos mPlaceChunkPos;
};