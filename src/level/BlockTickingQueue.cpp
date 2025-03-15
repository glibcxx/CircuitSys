#include "BlockTickingQueue.h"

#include <random>
#include "block/Block.h"
#include "chunk/LevelChunk.h"
#include "block/BlockLegacy.h"
#include "BlockSource.h"
#include "deps/Math.h"

bool BlockTickingQueue::BlockTick::operator==(const BlockTickingQueue::BlockTick &blockTick) const
{
    return this->mIsRemoved == blockTick.mIsRemoved && this->mData == blockTick.mData;
}

bool BlockTickingQueue::BlockTick::operator<(const BlockTickingQueue::BlockTick &blockTick) const
{
    return this->mData < blockTick.mData;
}

bool BlockTickingQueue::BlockTick::operator>(const BlockTickingQueue::BlockTick &blockTick) const
{
    return this->mData > blockTick.mData;
}

bool BlockTickingQueue::TickDataSet::remove(const BlockPos &pos, const Block &block)
{
    bool removedSomething = false;
    for (int index = this->size() - 1; index >= 0; --index)
    {
        if (this->mC[index].mData.pos == pos && this->mC[index].mData.mBlock->getLegacyBlock() == block.getLegacyBlock())
        {
            removedSomething = !this->mC[index].mIsRemoved || removedSomething;
            this->mC[index].mIsRemoved = true;
        }
    }
    return removedSomething;
}

bool BlockTickingQueue::TickDataSet::remove(std::function<bool(const TickNextTickData &)> &&removeCondition)
{
    bool removedSomething = false;
    for (int index = this->mC.size() - 1; index >= 0; --index)
    {
        BlockTickingQueue::BlockTick &tickData = this->mC[index];
        if (!tickData.mIsRemoved && removeCondition(tickData.mData))
        {
            removedSomething = true;
            this->mC[index].mIsRemoved = true;
        }
    }
    return removedSomething;
}

void BlockTickingQueue::setOwningChunk(LevelChunk *owningChunk)
{
    this->mOwningChunk = owningChunk;
}

void BlockTickingQueue::add(BlockSource &region, const BlockPos &pos, const Block &block, int tickDelay, int priorityOffset)
{
    // int      r = !this->mOwningChunk || this->mOwningChunk->getState() < ChunkState::PostProcessed || tickDelay < 0 ? 8 : 0;
    // BlockPos radius{r};
    // BlockPos max = pos - radius;
    // BlockPos min = pos + radius;
    if (/*region.hasChunksAt(min, max)*/ true)
    {
        if (tickDelay >= 0)
        {
            Tick tick = this->mCurrentTick + tickDelay;
            this->mNextTickQueue.emplace(pos, block, tick, priorityOffset);
            if (this->mOwningChunk)
                this->_onQueueChanged();
        }
        else
        {
            static std::mt19937 rng;

            const Block &blockAtPos = region.getBlock(pos);
            // if (blockAtPos.getLegacyBlock() == block.getLegacyBlock() && blockAtPos.getLegacyBlock() != *BedrockBlockTypes::mAir)
            // {
            //     blockAtPos.tick(region, pos, rng);
            // }
            // else
            // {
            //     const Block &extraBlock = region.getExtraBlock(pos);
            //     if (extraBlock != *BedrockBlocks::mAir)
            //     {
            //         if (extraBlock.getLegacyBlock() == block.getLegacyBlock())
            //         {
            //             block.tick(region, pos, rng);
            //         }
            //     }
            // }
        }
    }
}

void BlockTickingQueue::remove(const BlockPos &pos, const Block &block)
{
    if (this->mNextTickQueue.remove(pos, block) && this->mOwningChunk)
    {
        this->_onQueueChanged();
    }
}

void BlockTickingQueue::remove(std::function<bool(const TickNextTickData &)> &&removeCondition)
{
    if (this->mNextTickQueue.remove(std::move(removeCondition)) && this->mOwningChunk)
    {
        this->_onQueueChanged();
    }
}

bool BlockTickingQueue::tickPendingTicks(BlockSource &region, const Tick &until, int max /* 100 */, bool instaTick_)
{
    if (until.getTimeStamp() == Tick::MAX.getTimeStamp() || this->mCurrentTick != until)
    {
        this->mInstaTick = instaTick_;
        int count = std::min<int>(max, this->mNextTickQueue.size());
        this->mCurrentTick = until;
        int i = 0;
        while (!this->mNextTickQueue.empty() && i < count)
        {
            BlockTick topBlockTick = this->mNextTickQueue.top();
            if (topBlockTick.mIsRemoved)
            {
                this->mNextTickQueue.pop();
            }
            else
            {
                if (topBlockTick.mData.tick > until)
                    break;
                ++i;
                this->mNextTickQueue.pop();
                this->mActiveTickQueue.emplace(std::move(topBlockTick));
            }
        }

        static std::mt19937 rng;
        while (!this->mActiveTickQueue.empty())
        {
            BlockTick topBlockTick = this->mActiveTickQueue.top();
            BlockPos  radius{8};
            BlockPos  min = topBlockTick.mData.pos - radius;
            BlockPos  max = topBlockTick.mData.pos + radius;

            if (/*region.hasChunksAt(min, max)*/ true)
            {
                const Block &blockFoundByPos = region.getBlock(topBlockTick.mData.pos);
                // if (blockFoundByPos.getLegacyBlock() == topBlockTick.mData.mBlock->getLegacyBlock() && blockFoundByPos.getLegacyBlock() != *BedrockBlockTypes::mAir)
                // {
                //     blockFoundByPos.tick(region, topBlockTick.mData.pos, rng);
                // }

                // const Block &extraBlock = region.getExtraBlock(topBlockTick.mData.pos);
                // if (extraBlock != *BedrockBlocks::mAir && extraBlock.getLegacyBlock() == topBlockTick.mData.mBlock->getLegacyBlock())
                // {
                //     topBlockTick.mData.mBlock->tick(region, topBlockTick.mData.pos, rng);
                // }
            }
            else
            {
                this->add(region, topBlockTick.mData.pos, *topBlockTick.mData.mBlock, 0, topBlockTick.mData.priorityOffset);
            }
            this->mActiveTickQueue.pop();
        }
        this->mInstaTick = false;
        if (this->mOwningChunk && i > 0)
            this->_onQueueChanged();
        return i > 0;
    }
    else
    {
        this->mInstaTick = false;
        return false;
    }
}

void BlockTickingQueue::tickAllPendingTicks(BlockSource &region)
{
    while (this->mNextTickQueue.size())
        this->tickPendingTicks(region, Tick::MAX, 0x7FFFFFFF, true);
}

bool BlockTickingQueue::isInstaticking() const
{
    return this->mInstaTick;
}

bool BlockTickingQueue::isEmpty() const
{
    return this->mNextTickQueue.empty();
}

bool BlockTickingQueue::hasTickInCurrentTick(const BlockPos &pos) const
{
    return std::find_if(this->mActiveTickQueue.begin(),
                        this->mActiveTickQueue.end(),
                        [&pos](const BlockTickingQueue::BlockTick &data) -> bool {
                            return !data.mIsRemoved && data.mData.pos == pos;
                        })
        != this->mActiveTickQueue.end();
}

bool BlockTickingQueue::hasTickInPendingTicks(const BlockPos &pos) const
{
    return std::find_if(this->mNextTickQueue.begin(),
                        this->mNextTickQueue.end(),
                        [&pos](const BlockTickingQueue::BlockTick &data) -> bool {
                            return !data.mIsRemoved && data.mData.pos == pos;
                        })
        != this->mNextTickQueue.end();
}

int BlockTickingQueue::ticksFromNow(int offset) const
{
    return offset + this->mCurrentTick.getTimeStamp();
}

bool BlockTickingQueue::getNextUpdateForPos(const BlockPos &pos, Tick &tick) const
{
    auto found = std::find_if(this->mActiveTickQueue.begin(), this->mActiveTickQueue.end(),
                              [&pos](const BlockTickingQueue::BlockTick &data) -> bool {
                                  return data.mData.pos == pos;
                              });
    if (found != this->mActiveTickQueue.end())
    {
        tick = found->mData.tick;
        return true;
    }
    found = std::find_if(this->mNextTickQueue.begin(), this->mNextTickQueue.end(),
                         [&pos](const BlockTickingQueue::BlockTick &data) -> bool {
                             return data.mData.pos == pos;
                         });

    if (found != this->mNextTickQueue.end())
    {
        tick = found->mData.tick;
        return true;
    }

    return false;
}