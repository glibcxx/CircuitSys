#pragma once

#include "BlockPos.h"
#include "Tick.h"

class Block;

class TickNextTickData
{
public:
    BlockPos     pos;
    const Block *mBlock;
    Tick         tick;
    int          priorityOffset;

    TickNextTickData(const BlockPos &pos, const Block &block, const Tick &tick, int pOffset) :
        pos(pos), mBlock(&block), tick(tick), priorityOffset(pOffset) {}

    bool operator==(const TickNextTickData &t) const;
    bool operator<(const TickNextTickData &tnd) const;
    bool operator>(const TickNextTickData &tnd) const;
};
