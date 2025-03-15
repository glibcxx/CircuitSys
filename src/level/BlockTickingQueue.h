#pragma once

#include <functional>
#include "TickNextTickData.h"
#include "deps/MovePriorityQueue.h"

class LevelChunk;
class BlockPalette;
class Block;
class BlockLegacy;
class BlockSource;

enum class TickingQueueType : uint8_t
{
    Internal = 0,
    Random = 1,
};

struct TickDelayBlock
{
    int          mTickDelay;
    const Block *mBlock;
};

class BlockTickingQueue
{
public:
    BlockTickingQueue() = delete;

    BlockTickingQueue(TickingQueueType queueType) :
        mQueueType(queueType), mInstaTick(false) {}

    BlockTickingQueue(const Tick start, TickingQueueType queueType) :
        mCurrentTick(start), mQueueType(queueType), mInstaTick(false) {}

    BlockTickingQueue(LevelChunk &owningChunk, TickingQueueType queueType) :
        mOwningChunk(&owningChunk), mQueueType(queueType), mInstaTick(false) {}

    static int const MAX_TICK_TILES_PER_TICK = 100;

    void setOwningChunk(LevelChunk *owningChunk);

    void add(BlockSource &region, const BlockPos &pos, const Block &block, int tickDelay, int priorityOffset);

    void remove(const BlockPos &pos, const Block &block);

    void remove(std::function<bool(const TickNextTickData &)> &&removeCondition);

    bool tickPendingTicks(BlockSource &region, const Tick &until, int max, bool instaTick_);

    void tickAllPendingTicks(BlockSource &region);

    bool isInstaticking() const;

    bool isEmpty() const;

    bool hasTickInCurrentTick(const BlockPos &pos) const;

    bool hasTickInPendingTicks(const BlockPos &pos) const;

    int ticksFromNow(int offset) const;

    bool getNextUpdateForPos(const BlockPos &pos, Tick &tick) const;

private:
    void _onQueueChanged() const {}

protected:
    class BlockTick
    {
    public:
        BlockTick(const BlockPos &pos, const Block &block, const Tick &tick, int priorityOffset = 0) :
            mData(pos, block, tick, priorityOffset) {}

        bool operator==(const BlockTick &blockTick) const;

        bool operator<(const BlockTick &blockTick) const;

        bool operator>(const BlockTick &blockTick) const;

        bool             mIsRemoved = false;
        TickNextTickData mData;
    };

    class TickDataSet : public MovePriorityQueue<BlockTick, std::greater<BlockTick>>
    {
    public:
        bool remove(const BlockPos &pos, const Block &block);

        bool remove(std::function<bool(const TickNextTickData &)> &&removeCondition);
    };

    LevelChunk      *mOwningChunk = nullptr;
    Tick             mCurrentTick{};
    TickDataSet      mNextTickQueue{};
    TickDataSet      mActiveTickQueue{};
    TickingQueueType mQueueType;
    bool             mInstaTick;
};