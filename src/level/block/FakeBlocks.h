#pragma once

#include "SharedPtr.h"

class Block;

namespace FakeBlocks
{
    void registryBlocks();
    void unregistryBlocks();

    extern const Block *mAir;
    extern const Block *mQuartzBlock;
    extern const Block *mRedstoneWireBlock;
    extern const Block *mObserverBlock;
    extern const Block *mRepeaterBlock;
    extern const Block *mTopSlabBlock;

} // namespace FakeBlocks