#include "FakeBlocks.h"
#include "Block.h"
#include <type_traits>

std::unordered_map<std::string, SharedPtr<BlockLegacy>> blockLegacyMap;
std::vector<std::unique_ptr<Block>>                     fakeBlocks;

class AirBlock : public BlockLegacy
{
    using BlockLegacy::BlockLegacy;
};

class QuartzBlock : public BlockLegacy
{
public:
    QuartzBlock(const std::string &nameId, int id) :
        BlockLegacy(nameId, id)
    {
        this->mSolid = true;
    }
};

class RedStoneWireBlock : public BlockLegacy
{
    using BlockLegacy::BlockLegacy;
};

class ObserverBlock : public BlockLegacy
{
    using BlockLegacy::BlockLegacy;

    virtual void tick(BlockSource &region, const BlockPos &pos, std::mt19937 &random)
    {
        // if (dimension.isRedstoneTick())
        // {
        //     CircuitSystem  &circuitSystem = dimension.getCircuitSystem();
        //     PulseCapacitor *component = circuitSystem.getComponent<PulseCapacitor>(pos);
        //     const Block    &block = region.getBlock(pos);
        //     int             powered = block.getVariant(); // 0: off, 1: on
        //     if (component)
        //         this->_updateState(region, pos, component, powered == 0);
        // }
        // else
        // {
        //     const Block &DefaultState = this->getDefaultState();
        //     region.addToTickingQueue(pos, DefaultState, 1, 0);
        // }
    }
};

class RepeaterBlock : public BlockLegacy
{
    using BlockLegacy::BlockLegacy;
};

class TopSlabBlock : public BlockLegacy
{
public:
    TopSlabBlock(const std::string &nameId, int id) :
        BlockLegacy(nameId, id)
    {
        this->mProperties |= BlockProperty::Power_BlockDown | BlockProperty::Power_NO;
    }
};

namespace FakeBlocks
{
    const Block *mAir;
    const Block *mQuartzBlock;
    const Block *mRedstoneWireBlock;
    const Block *mObserverBlock;
    const Block *mRepeaterBlock;
    const Block *mTopSlabBlock;

} // namespace FakeBlocks

template <typename BlockType, std::enable_if_t<std::is_base_of_v<BlockLegacy, BlockType>, std::nullptr_t> = nullptr>
const Block &_registryBlock(const std::string &name)
{
    auto  iter = blockLegacyMap.emplace(name, SharedPtr<BlockType>::make(name, (int)blockLegacyMap.size()));
    auto &block = fakeBlocks.emplace_back(std::make_unique<Block>(iter.first->second));
    return *block;
}

void FakeBlocks::registryBlocks()
{
    FakeBlocks::mAir = &_registryBlock<AirBlock>("air");
    FakeBlocks::mQuartzBlock = &_registryBlock<QuartzBlock>("quartz_block");
    FakeBlocks::mRedstoneWireBlock = &_registryBlock<RedStoneWireBlock>("redstone_wire_block");
    FakeBlocks::mObserverBlock = &_registryBlock<ObserverBlock>("observer_block");
    FakeBlocks::mRepeaterBlock = &_registryBlock<RepeaterBlock>("repeater_block");
    FakeBlocks::mTopSlabBlock = &_registryBlock<TopSlabBlock>("quartz_slab");
}

void FakeBlocks::unregistryBlocks()
{
    blockLegacyMap.clear();
}
