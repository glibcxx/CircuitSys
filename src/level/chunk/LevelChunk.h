#pragma once

#include "level/ChunkPos.h"

class LevelChunk
{
public:
    const ChunkPos &getPosition() const { return this->mPosition; }

    ChunkPos mPosition;
};
