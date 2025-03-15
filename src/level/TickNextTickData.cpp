#include "TickNextTickData.h"

#include "level/block/Block.h"
#include "level/block/BlockLegacy.h"

bool TickNextTickData::operator==(const TickNextTickData &t) const
{
    return this->pos == t.pos && this->mBlock->getLegacyBlock() == t.mBlock->getLegacyBlock();
}

bool TickNextTickData::operator<(const TickNextTickData &tnd) const
{
    if (this->tick < tnd.tick || this->tick > tnd.tick)
        return this->tick < tnd.tick;
    else
        return this->priorityOffset < tnd.priorityOffset;
}

bool TickNextTickData::operator>(const TickNextTickData &tnd) const
{
    if (this->tick < tnd.tick || this->tick > tnd.tick)
        return this->tick > tnd.tick;
    else
        return this->priorityOffset > tnd.priorityOffset;
}