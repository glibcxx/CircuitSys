#include "CircuitSystem.h"

#include "level/block/Block.h"
#include "level/BlockSource.h"
#include "CircuitTrackingInfo.h"

void CircuitSystem::updateDependencies(BlockSource *region)
{
    this->mSceneGraph.update(region);
    this->mHasBeenEvaluated = false;
}

void CircuitSystem::evaluate(BlockSource *region)
{
    this->_shouldEvaluate(region);
    this->cacheValues();
    this->evaluateComponents(true);
    this->evaluateComponents(false);
    this->checkLocks();
    this->mHasBeenEvaluated = true;
}

void CircuitSystem::_shouldEvaluate(BlockSource *source)
{
    if (!source)
        return;
    CircuitSceneGraph::ComponentsPerPosMap
        componentPerChunkMap = this->mSceneGraph.getComponents_FastLookupByChunkPos();
    for (auto &&chunks : componentPerChunkMap)
    {
        bool shouldEvaluate = source->areChunksFullyLoaded(chunks.first, 32);
        for (auto &&comp : chunks.second)
        {
            if (comp.mComponent)
            {
                comp.mComponent->mShouldEvaluate = shouldEvaluate && !comp.mComponent->isRemoved();
            }
        }
    }
}

void CircuitSystem::cacheValues()
{
    CircuitComponentList &componentMap = this->mSceneGraph.getComponents_FastIterationAcrossActive();
    for (auto componentIter = componentMap.begin(); componentIter != componentMap.end(); ++componentIter)
    {
        BaseCircuitComponent *component = componentIter->mComponent;
        if (component->mShouldEvaluate)
        {
            BlockPos position = componentIter->mPos;
            component->cacheValues(*this, position);
        }
    }
}

void CircuitSystem::checkLocks()
{
    CircuitComponentList &componentMap = this->mSceneGraph.getComponents_FastIterationAcrossActive();
    for (auto componentIter = componentMap.begin(); componentIter != componentMap.end(); ++componentIter)
    {
        BaseCircuitComponent *component = componentIter->mComponent;
        if (component->mShouldEvaluate)
        {
            BlockPos position = componentIter->mPos;
            component->checkLock(*this, position);
        }
    }
}

void CircuitSystem::evaluateComponents(bool bOnlyProducers)
{
    CircuitComponentList &componentMap = this->mSceneGraph.getComponents_FastIterationAcrossActive();
    for (auto componentIter = componentMap.begin(); componentIter != componentMap.end(); ++componentIter)
    {
        BaseCircuitComponent *component = componentIter->mComponent;
        CircuitComponentType  typeID = component->getCircuitComponentGroupType();
        if (component->mShouldEvaluate)
        {
            if (bOnlyProducers == (typeID == CircuitComponentType::ProducerComponent || typeID == CircuitComponentType::CapacitorComponent))
            {
                BlockPos position = componentIter->mPos;
                component->mNeedsUpdate = component->evaluate(*this, position);
            }
        }
    }
}

FacingID CircuitSystem::getDirection(const BlockPos &pos)
{
    BaseCircuitComponent *component = this->mSceneGraph.getBaseComponent(pos);
    return component ? component->getDirection() : Facing::NOT_DEFINED;
}

int CircuitSystem::getStrength(const BlockPos &pos)
{
    BaseCircuitComponent *component = this->mSceneGraph.getBaseComponent(pos);
    return component ? component->getStrength() : -1;
}

bool CircuitSystem::hasDirectPower(const BlockPos &pos)
{
    BaseCircuitComponent *component = this->mSceneGraph.getBaseComponent(pos);
    return component && component->hasDirectPower();
}

void CircuitSystem::invalidatePos(const BlockPos &pos)
{
    if (!this->mLockGraph)
        this->mSceneGraph.invalidatePos(pos);
}

bool CircuitSystem::isAvailableAt(const BlockPos &pos)
{
    return this->mSceneGraph.getBaseComponent(pos) != nullptr;
}

bool CircuitSystem::isConnectionAllowed(BaseCircuitComponent &component, const BlockPos &pos, FacingID face, bool bDirectlyPowered)
{
    BaseCircuitComponent *comp = this->mSceneGraph.getBaseComponent(pos.relative(face, 1));
    if (comp)
        return comp->allowConnection(this->mSceneGraph, {&component, pos, 0}, bDirectlyPowered);
    return comp;
}

void CircuitSystem::lockGraph(bool bLock)
{
    this->mLockGraph = bLock;
}

const BlockPos CircuitSystem::getChunkPos(BlockSource *region, const BlockPos &pos)
{
    return {region->getChunkAt(pos)->getPosition(), 0};
}

void CircuitSystem::preSetupPoweredBlocks(const ChunkPos &chunkPos)
{
    if (!this->mLockGraph)
        this->mSceneGraph.preSetupPoweredBlocks(chunkPos);
}

void CircuitSystem::removeComponents(const BlockPos &pos)
{
    if (!this->mLockGraph)
    {
        BaseCircuitComponent *component = this->mSceneGraph.getBaseComponent(pos);
        this->mSceneGraph.remove(pos, component);
    }
}

void CircuitSystem::setStrength(const BlockPos &pos, int strength)
{
    BaseCircuitComponent *component = this->mSceneGraph.getBaseComponent(pos);
    if (component)
        component->setStrength(strength);
}

BaseCircuitComponent *CircuitSystem::createComponent(const BlockPos &pos, FacingID direction, std::unique_ptr<BaseCircuitComponent> newComponent)
{
    BaseCircuitComponent *component = newComponent.get();
    component->setDirection(direction);
    if (this->mLockGraph)
    {
        newComponent.reset(component);
        return nullptr;
    }
    else
    {
        this->mSceneGraph.add(pos, std::move(newComponent));
        return this->mSceneGraph.getFromPendingAdd(pos);
    }
}

void CircuitSystem::updateBlocks(BlockSource &region, const BlockPos &chunkPos)
{
    if (this->mHasBeenEvaluated)
    {
        auto &componentPerChunkMap = this->mSceneGraph.getComponents_FastLookupByChunkPos();
        if (componentPerChunkMap.size())
        {
            auto chunkList = componentPerChunkMap.find(chunkPos);
            if (chunkList != componentPerChunkMap.end())
            {
                CircuitComponentList  secondaryPoweredList;
                CircuitComponentList &componentMap = chunkList->second;

                auto componentIter = componentMap.mComponents.begin();
                for (; componentIter != componentMap.mComponents.end(); ++componentIter)
                {
                    BaseCircuitComponent *component = componentIter->mComponent;
                    if (component->mNeedsUpdate && !component->isRemoved())
                    {
                        component->mNeedsUpdate = false;
                        if (component->isSecondaryPowered())
                            secondaryPoweredList.push_back(*componentIter);
                        else
                            this->updateIndividualBlock(component, chunkPos, componentIter->mPos, region);
                    }
                }
                for (componentIter = secondaryPoweredList.begin(); componentIter != secondaryPoweredList.end(); ++componentIter)
                {
                    this->updateIndividualBlock(componentIter->mComponent, chunkPos, componentIter->mPos, region);
                }
            }
        }
    }
}

void CircuitSystem::updateIndividualBlock(gsl::not_null<BaseCircuitComponent *> component, const BlockPos &chunkPos, const BlockPos &pos, BlockSource &region)
{
    int strength = component->getStrength();
    if (strength != CircuitSystem::NOT_DEFINED_STRENGTH)
    {
        const Block &block = region.getBlock(pos);
        if (!component->mIsFirstTime || !component->mIgnoreFirstUpdate)
            block.onRedstoneUpdate(region, pos, strength, component->mIsFirstTime);
        component->mIsFirstTime = false;
    }
}