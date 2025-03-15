#include "CircuitSceneGraph.h"
#include <queue>
#include "level/BlockSource.h"
#include "level/block/Block.h"
#include "components/PoweredBlockComponent.h"
#include "components/TransporterComponent.h"
#include "components/BaseRailTransporter.h"

void addToFillQueue(CircuitSceneGraph               &graph,
                    CircuitComponentList            &powerAssociationMap,
                    BaseCircuitComponent            *newComponent,
                    CircuitTrackingInfo             &info,
                    const BlockPos                  &newPos,
                    FacingID                         face,
                    std::queue<CircuitTrackingInfo> &queue)
{
    if (!newComponent)
        return;

    CircuitTrackingInfo newInfo = info;
    newInfo.mCurrent.set(newComponent, newPos, face, newComponent->getCircuitComponentGroupType());
    int  newDampening = info.mDampening;
    bool bDirectlyPowered = info.mDirectlyPowered;

    powerAssociationMap.add(newComponent, 0, newPos);

    if (info.mNearest.mComponent->allowConnection(graph, newInfo, bDirectlyPowered))
    {
        if (newComponent->addSource(graph, newInfo, newDampening, bDirectlyPowered))
        {
            newInfo.m2ndNearest = info.mNearest;
            newInfo.mNearest.set(newComponent, newPos, face, newInfo.mCurrent.mTypeID);
            newInfo.mDampening = newDampening;
            newInfo.mDirectlyPowered = bDirectlyPowered;
            queue.push(newInfo);
        }
    }
}

void searchForRelationshipAt(BlockSource                     *bs,
                             CircuitComponentList            &powerAssociationMap,
                             CircuitSceneGraph               &graph,
                             CircuitTrackingInfo             &trackInfo,
                             FacingID                         facing,
                             std::queue<CircuitTrackingInfo> &queue)
{
    BlockPos newPos = trackInfo.mNearest.mPos + Facing::DIRECTION[facing];

    BaseCircuitComponent *newComponent = graph.getBaseComponent(newPos);
    if (newPos != trackInfo.mNearest.mPos && newPos != trackInfo.m2ndNearest.mPos)
    {
        addToFillQueue(graph, powerAssociationMap, newComponent, trackInfo, newPos, facing, queue);
    }
}

void checkComponent(CircuitSceneGraph               &graph,
                    CircuitComponentList            &powerAssociationMap,
                    FacingID                         id,
                    const BlockPos                  &otherPos,
                    CircuitTrackingInfo             &info,
                    std::queue<CircuitTrackingInfo> &positions,
                    CircuitComponentType             type)
{
    BlockPos              targetPos = otherPos + Facing::DIRECTION[id];
    BaseCircuitComponent *Component = graph.getComponent(targetPos, type);
    addToFillQueue(graph, powerAssociationMap, Component, info, targetPos, id, positions);
}

void searchForRelationhshipsAboveAndBelow(CircuitSceneGraph               &graph,
                                          CircuitComponentList            &powerAssociationMap,
                                          BaseCircuitComponent            *currentComponent,
                                          BlockPos                         targetPos,
                                          CircuitTrackingInfo             &info,
                                          FacingID                         dir,
                                          std::queue<CircuitTrackingInfo> &positions)
{
    auto typeID = currentComponent->getCircuitComponentGroupType();
    if (typeID == CircuitComponentType::TransporterComponent)
    {
        BlockPos              otherPos = targetPos + Facing::DIRECTION[1];
        BaseCircuitComponent *up = graph.getComponent(otherPos, CircuitComponentType::PoweredBlockComponent);
        if (!up || up->canAllowPowerUp())
        {
            for (auto facing = 2; facing <= 5; facing++)
            {
                checkComponent(graph, powerAssociationMap, facing, otherPos, info, positions, CircuitComponentType::TransporterComponent);
            }
        }
        otherPos = targetPos + Facing::DIRECTION[0];
        BaseCircuitComponent *down = graph.getComponent(otherPos, CircuitComponentType::PoweredBlockComponent);
        if (!down || !down->canAllowPowerUp())
        {
            for (auto facing = 2; facing <= 5; facing++)
            {
                checkComponent(graph, powerAssociationMap, facing, otherPos, info, positions, CircuitComponentType::TransporterComponent);
            }
        }
    }
    else if (typeID != CircuitComponentType::ProducerComponent || currentComponent->getDirection() != Facing::DOWN)
    {
        if (currentComponent->getCircuitComponentType() == CircuitComponentType::BaseRailTransporter)
        {
            BlockPos otherPos = targetPos + Facing::DIRECTION[1];
            for (auto facing = 2; facing <= 5; facing++)
            {
                checkComponent(graph, powerAssociationMap, facing, otherPos, info, positions, CircuitComponentType::BaseRailTransporter);
            }
            otherPos = targetPos + Facing::DIRECTION[0];
            for (auto facing = 2; facing <= 5; facing++)
            {
                checkComponent(graph, powerAssociationMap, facing, otherPos, info, positions, CircuitComponentType::BaseRailTransporter);
            }
        }
    }
    else
    {
        BlockPos               topPos = targetPos + Facing::DIRECTION[1];
        BlockPos               downPos = targetPos + Facing::DIRECTION[0];
        PoweredBlockComponent *topComp = static_cast<PoweredBlockComponent *>(graph.getComponent(topPos, CircuitComponentType::PoweredBlockComponent));
        PoweredBlockComponent *downComp = static_cast<PoweredBlockComponent *>(graph.getComponent(downPos, CircuitComponentType::PoweredBlockComponent));
        if ((topComp || downComp)
            && (!downComp || downComp->isAllowedAsPowerSource())
            && (!topComp || topComp->isAllowedAsPowerSource()))
        {
            if (!(downComp && downComp->canAllowPowerUp())
                && (!topComp || topComp->canAllowPowerUp() || topComp->isPromotedToProducer()))
            {
                for (auto facing = 2; facing <= 5; facing++)
                {
                    checkComponent(graph, powerAssociationMap, facing, downPos, info, positions, CircuitComponentType::TransporterComponent);
                }
            }
        }
    }
}

void CircuitSceneGraph::add(const BlockPos &pos, std::unique_ptr<BaseCircuitComponent> component)
{
    auto iter = this->mPendingAdds.find(pos);
    if (iter != this->mPendingAdds.end())
    {
        CircuitSceneGraph::PendingEntry &pending = iter->second;
        if (pending.mComponent->getCircuitComponentGroupType() == CircuitComponentType::PoweredBlockComponent)
        {
            BaseCircuitComponent *poweredComponent = pending.mComponent.get();
            if (!poweredComponent->canAllowPowerUp())
            {
                pending.mComponent.reset(nullptr);
                pending.mComponent = std::move(component);
            }
            return;
        }
        if (component->getCircuitComponentGroupType() == CircuitComponentType::PoweredBlockComponent)
        {
            BaseCircuitComponent *poweredComponent = component.get();
            if (poweredComponent->canAllowPowerUp())
            {
                pending.mComponent.reset(nullptr);
                pending.mComponent = std::move(component);
            }
            return;
        }
    }
    this->mPendingAdds.insert({pos, PendingEntry(pos, std::move(component))});
    return;
}

BaseCircuitComponent *CircuitSceneGraph::getBaseComponent(const BlockPos &pos)
{
    auto find = this->mAllComponents.find(pos);
    if (find != this->mAllComponents.end())
    {
        BaseCircuitComponent *systemComponent = find->second.get();
        if (!systemComponent->isRemoved())
            return systemComponent;
    }
    return nullptr;
}

BaseCircuitComponent *CircuitSceneGraph::getComponent(const BlockPos &pos, CircuitComponentType typeID)
{
    auto find = this->mAllComponents.find(pos);
    if (find != mAllComponents.end())
    {
        BaseCircuitComponent *systemComponent = find->second.get();
        if (!systemComponent->isRemoved())
        {
            CircuitComponentType idBase = systemComponent->getCircuitComponentGroupType();
            CircuitComponentType id = systemComponent->getCircuitComponentType();
            CircuitComponentType idComponent = typeID;
            if (idComponent == id || idComponent == idBase)
                return systemComponent;
        }
    }
    return nullptr;
}

CircuitSceneGraph::ComponentMap &CircuitSceneGraph::getComponents_FastLookupByPos()
{
    return this->mAllComponents;
}

CircuitComponentList &CircuitSceneGraph::getComponents_FastIterationAcrossActive()
{
    return this->mActiveComponents;
}

CircuitSceneGraph::ComponentsPerPosMap &CircuitSceneGraph::getComponents_FastLookupByChunkPos()
{
    return this->mActiveComponentsPerChunk;
}

BaseCircuitComponent *CircuitSceneGraph::getFromPendingAdd(const BlockPos &pos, CircuitComponentType typeID)
{
    BaseCircuitComponent *systemComponent = this->getFromPendingAdd(pos);
    if (systemComponent)
    {
        CircuitComponentType idBase = systemComponent->getCircuitComponentGroupType();
        CircuitComponentType id = systemComponent->getCircuitComponentType();
        CircuitComponentType idComponent = typeID;
        if (idComponent == id || idComponent == idBase)
            return systemComponent;
    }
    return nullptr;
}

BaseCircuitComponent *CircuitSceneGraph::getFromPendingAdd(const BlockPos &pos)
{
    auto iter = this->mPendingAdds.find(pos);
    return iter != mPendingAdds.end() ? iter->second.mComponent.get() : nullptr;
}

void CircuitSceneGraph::setPendingAddAsNewlyLoaded()
{
    for (auto iter = this->mPendingAdds.begin(); iter != this->mPendingAdds.end(); iter++)
    {
        iter->second.mComponent->mIsFirstTime = true;
    }
}

void CircuitSceneGraph::preSetupPoweredBlocks(const ChunkPos &chunkPos)
{
    auto chunkPosIter = this->mComponentsToReEvaluate.find({chunkPos, 0});
    if (chunkPosIter != mComponentsToReEvaluate.end())
    {
        auto &blocks = chunkPosIter->second;
        for (auto &block : blocks)
        {
            auto comp = this->getBaseComponent(block);
            this->scheduleRelationshipUpdate(block, comp);
        }
        this->mComponentsToReEvaluate.erase(chunkPosIter);
    }
}

void CircuitSceneGraph::scheduleRelationshipUpdate(const BlockPos &pos, BaseCircuitComponent *component)
{
    if (!component)
        return;
    CircuitComponentType typeID = component->getCircuitComponentGroupType();
    if (typeID == CircuitComponentType::CapacitorComponent || typeID == CircuitComponentType::ProducerComponent)
    {
        if (this->mPendingUpdates.find(pos) == this->mPendingUpdates.end())
        {
            this->mPendingUpdates.insert({pos, PendingEntry(pos, component)});
        }
    }
}

void CircuitSceneGraph::update(BlockSource *pSource)
{
    this->processPendingRemoves();
    this->processPendingAdds();
    this->processPendingUpdates(pSource);
}

void CircuitSceneGraph::remove(const BlockPos &pos, BaseCircuitComponent *component)
{
    auto iter = this->mPendingAdds.find(pos);
    if (iter != this->mPendingAdds.end())
        this->mPendingAdds.erase(pos);
    if (component)
    {
        component->setRemoved();
        this->mPendingRemoves.push_back({pos, component});
    }
}

bool CircuitSceneGraph::isPendingAdd(const BlockPos &pos)
{
    auto iter = this->mPendingAdds.find(pos);
    return iter != this->mPendingAdds.end();
}

void CircuitSceneGraph::processPendingAdds()
{
    for (auto iter = this->mPendingAdds.begin(); iter != mPendingAdds.end(); iter = this->mPendingAdds.erase(iter))
    {
        PendingEntry         &pendingToAdd = iter->second;
        BaseCircuitComponent *component = iter->second.mComponent.get();
        BlockPos              pos = pendingToAdd.mPos;

        ComponentMap::iterator componentGroupIter = this->mAllComponents.find(pos);
        bool                   updatedType = false;
        if (componentGroupIter != mAllComponents.end() && componentGroupIter->second)
        {
            if (componentGroupIter->second->getCircuitComponentGroupType() == CircuitComponentType::PoweredBlockComponent)
            {
                updatedType = component->getCircuitComponentGroupType() != CircuitComponentType::PoweredBlockComponent;
            }
        }
        if (componentGroupIter != mAllComponents.end() ? updatedType : true)
        {
            if (updatedType)
            {
                for (auto &pair : this->mAllComponents)
                {
                    if (pair.second->hasSource(*componentGroupIter->second))
                    {
                        pair.second->removeSource(componentGroupIter->first, componentGroupIter->second.get());
                    }
                }

                this->mAllComponents.erase(componentGroupIter);
            }
            this->mAllComponents.insert({pos, std::move(pendingToAdd.mComponent)});
            CircuitComponentType typeID = component->getCircuitComponentGroupType();
            if (typeID != CircuitComponentType::PoweredBlockComponent)
            {
                this->mActiveComponents.add(component, 0, pos);

                auto chunkList = this->mActiveComponentsPerChunk.find(component->mChunkPosition);
                if (chunkList == this->mActiveComponentsPerChunk.end())
                {
                    CircuitComponentList base{};
                    this->mActiveComponentsPerChunk.insert({component->mChunkPosition, base});
                }

                this->mActiveComponentsPerChunk[component->mChunkPosition].add(component, 0, pos);
            }
            if (typeID == CircuitComponentType::ProducerComponent)
            {
                this->scheduleRelationshipUpdate(pos, component);
            }
            else if (typeID == CircuitComponentType::CapacitorComponent)
            {
                this->scheduleRelationshipUpdate(pos, component);
                for (int corner1 = -1; corner1 <= 1; corner1 += 2)
                {
                    for (int corner2 = -1; corner2 <= 1; corner2 += 2)
                    {
                        BlockPos              otherPos = pos + BlockPos(corner1, 0, corner2);
                        BaseCircuitComponent *possibleComponent = this->getBaseComponent(otherPos);
                        if (possibleComponent && possibleComponent->getCircuitComponentGroupType() == CircuitComponentType::CapacitorComponent)
                        {
                            this->scheduleRelationshipUpdate(otherPos, possibleComponent);
                        }
                    }
                }
            }

            for (int iLevel = -1; iLevel <= 1; iLevel++)
            {
                for (int face = 0; face < 6; face++)
                {
                    BlockPos otherPos = pos + Facing::DIRECTION[face];
                    otherPos.y += iLevel;
                    BaseCircuitComponent *possibleComponent = this->getBaseComponent(otherPos);
                    if (possibleComponent)
                    {
                        CircuitComponentType typeID = possibleComponent->getCircuitComponentGroupType();
                        if (typeID == CircuitComponentType::ProducerComponent || typeID == CircuitComponentType::CapacitorComponent)
                        {
                            this->scheduleRelationshipUpdate(otherPos, possibleComponent);
                        }
                        else
                            for (auto &iterSource : possibleComponent->mSources)
                            {
                                BaseCircuitComponent *targetSourceComponent = this->getBaseComponent(iterSource.mPos);
                                this->scheduleRelationshipUpdate(iterSource.mPos, targetSourceComponent);
                            }
                    }
                }
            }
        }
    }
}

void CircuitSceneGraph::processPendingRemoves()
{
    for (auto iter = this->mPendingRemoves.begin(); iter != this->mPendingRemoves.end(); iter = this->mPendingRemoves.erase(iter))
    {
        this->removeComponent(iter->mPos);
    }
}

void CircuitSceneGraph::processPendingUpdates(BlockSource *region)
{
    if (this->mPendingUpdates.size())
    {
        this->removeStaleRelationships();

        for (auto iter = this->mPendingUpdates.begin(); iter != this->mPendingUpdates.end(); iter = this->mPendingUpdates.erase(iter))
        {
            if (iter->second.mRawComponentPtr)
            {
                this->findRelationships(iter->second.mPos, iter->second.mRawComponentPtr, region);
            }
        }

        for (auto &iterComponent : this->mAllComponents)
        {
            iterComponent.second->updateDependencies(*this, iterComponent.first);
        }
    }
}

void CircuitSceneGraph::removeStaleRelationships()
{
    for (auto iterUpdate = this->mPendingUpdates.begin(); iterUpdate != this->mPendingUpdates.end(); iterUpdate++)
    {
        BlockPos posUpdate = iterUpdate->second.mPos;
        auto     powerAssociationIter = this->mPowerAssociationMap.find(posUpdate);
        if (powerAssociationIter != this->mPowerAssociationMap.end())
        {
            CircuitComponentList &relationships = powerAssociationIter->second;
            for (auto perChunkIter = relationships.begin(); perChunkIter != relationships.end();)
            {
                auto allListIter = this->mAllComponents.find(perChunkIter->mPos);
                if (allListIter != mAllComponents.end())
                {
                    allListIter->second->removeSource(posUpdate, iterUpdate->second.mRawComponentPtr);
                    perChunkIter = relationships.mComponents.erase(perChunkIter);
                }
                else
                {
                    perChunkIter++;
                }
            }
        }
    }
}

void CircuitSceneGraph::findRelationships(const BlockPos &pos, BaseCircuitComponent *producerTarget, BlockSource *region)
{
    std::queue<CircuitTrackingInfo> stack;
    CircuitTrackingInfo             startInfo(producerTarget, pos, 0);

    auto powerAssociationIter = this->mPowerAssociationMap.find(pos);
    if (powerAssociationIter == mPowerAssociationMap.end())
    {
        this->mPowerAssociationMap.insert({pos, CircuitComponentList()});
    }
    powerAssociationIter = this->mPowerAssociationMap.find(pos);

    stack.push(startInfo);
    do
    {
        CircuitTrackingInfo fillTrack = stack.front();
        stack.pop();
        BlockPos              targetPos{fillTrack.mNearest.mPos};
        BaseCircuitComponent *currentComponent = fillTrack.mNearest.mComponent;
        if (currentComponent)
        {
            int      dampening = fillTrack.mDampening;
            FacingID dir = currentComponent->getDirection();
            if (dampening <= 15)
            {
                if (currentComponent->getCircuitComponentGroupType() != CircuitComponentType::PoweredBlockComponent)
                {
                    for (int facing = 0; facing < 6; facing++)
                    {
                        BlockPos newPos = targetPos + Facing::DIRECTION[facing];

                        if (!this->getBaseComponent(newPos) && region)
                        {
                            if (region->hasChunksAt(newPos, 0))
                            {
                                this->addIfPoweredBlockAt(*region, newPos);
                            }
                            else
                            {
                                ChunkPos chunkPos{newPos};
                                this->addPositionToReEvaluate(chunkPos, targetPos);
                            }
                        }
                    }
                }

                for (int face = 0; face < 6; face++)
                {
                    searchForRelationshipAt(region, powerAssociationIter->second, *this, fillTrack, face, stack);
                }

                searchForRelationhshipsAboveAndBelow(*this, powerAssociationIter->second,
                                                     currentComponent,
                                                     targetPos,
                                                     fillTrack,
                                                     dir,
                                                     stack);
            }
        }
    } while (!stack.empty());
}

BaseCircuitComponent *CircuitSceneGraph::addIfPoweredBlockAt(BlockSource &source, const BlockPos &pos)
{
    PoweredBlockComponent *poweredBlock = nullptr;
    const Block           &block = source.getBlock(pos);
    if (block != *FakeBlocks::mAir)
    {
        BlockProperty property = block.getRedstoneProperty(source, pos);
        if (block.isSolid() || (bool)(property & BlockProperty::Power_BlockDown))
        {
            std::unique_ptr unique_ptr_PB = std::make_unique<PoweredBlockComponent>();
            poweredBlock = unique_ptr_PB.get();
            this->mAllComponents.insert({pos, std::move(unique_ptr_PB)});
            if ((bool)(property & BlockProperty::Power_BlockDown))
                poweredBlock->setAllowPowerUp(true);
            if ((bool)(property & BlockProperty::Power_NO))
                poweredBlock->setAllowAsPowerSource(false);
        }
    }
    return poweredBlock;
}

void CircuitSceneGraph::invalidatePos(const BlockPos &pos)
{
    BaseCircuitComponent *targetComponent = this->getBaseComponent(pos);
    if (targetComponent)
    {
        for (auto &&iterSourceChild : targetComponent->mSources)
        {
            BaseCircuitComponent *targetChildComponent = this->getBaseComponent(iterSourceChild.mPos);
            this->scheduleRelationshipUpdate(iterSourceChild.mPos, targetChildComponent);
        }
    }
    else
    {
        for (int face = 0; face < 6; ++face)
        {
            BlockPos              currentPos = pos + Facing::DIRECTION[face];
            BaseCircuitComponent *component = this->getBaseComponent(currentPos);
            if (component)
            {
                this->scheduleRelationshipUpdate(currentPos, component);
                for (auto &&iterSource : component->mSources)
                {
                    BaseCircuitComponent *targetSourceComponent = this->getBaseComponent(iterSource.mPos);
                    this->scheduleRelationshipUpdate(iterSource.mPos, targetSourceComponent);
                }
            }
        }
    }
}

void CircuitSceneGraph::addPositionToReEvaluate(const ChunkPos &chunkPos, const BlockPos &pos)
{
    this->mComponentsToReEvaluate[BlockPos{chunkPos, 0}].emplace_back(pos);
}

void CircuitSceneGraph::removeComponent(const BlockPos &pos)
{
    auto componentGroupIter = this->mAllComponents.find(pos);
    if (componentGroupIter == mAllComponents.end())
        return;
    std::unique_ptr<BaseCircuitComponent> component(std::move(componentGroupIter->second));
    this->mAllComponents.erase(pos);
    if (component->getCircuitComponentGroupType() != CircuitComponentType::PoweredBlockComponent)
    {
        this->mActiveComponents.removeSource(pos, component.get());
        this->mPowerAssociationMap.erase(pos);
    }

    for (auto &perChunkIter : this->mActiveComponentsPerChunk)
    {
        perChunkIter.second.removeSource(pos, component.get());
    }

    for (auto &iterSource : component->mSources)
    {
        BaseCircuitComponent *updateComponent = iterSource.mComponent;
        this->scheduleRelationshipUpdate(iterSource.mPos, updateComponent);
    }

    for (auto &allComp : this->mAllComponents)
    {
        BaseCircuitComponent *updateComponent = allComp.second.get();
        updateComponent->removeSource(pos, component.get());
        auto iterSource = updateComponent->mSources.begin();
        while (iterSource != updateComponent->mSources.end())
        {
            if (iterSource->mPos == pos || iterSource->mComponent == component.get())
            {
                this->scheduleRelationshipUpdate(allComp.first, updateComponent);
                iterSource = updateComponent->mSources.erase(iterSource);
            }
            else
            {
                ++iterSource;
            }
        }
    }

    for (int face = 0; face < 6; ++face)
    {
        BlockPos              otherPos = pos + Facing::DIRECTION[face];
        BaseCircuitComponent *updateComponent = this->getBaseComponent(otherPos);
        if (updateComponent)
        {
            this->scheduleRelationshipUpdate(otherPos, updateComponent);
            for (auto &iterSource : updateComponent->mSources)
            {
                BaseCircuitComponent *sourcePower = this->getBaseComponent(iterSource.mPos);
                this->scheduleRelationshipUpdate(iterSource.mPos, sourcePower);
            }
        }
    }
    auto iterUpdate = this->mPendingUpdates.find(pos);
    if (iterUpdate != this->mPendingUpdates.end())
    {
        this->mPendingUpdates.erase(iterUpdate);
    }
    component.reset(nullptr);
}

CircuitSceneGraph::PendingEntry::PendingEntry(CircuitSceneGraph::PendingEntry &&entry)
{
    this->mComponent = std::move(entry.mComponent);
    this->mRawComponentPtr = entry.mRawComponentPtr;
    this->mPos = entry.mPos;
}

CircuitSceneGraph::PendingEntry::PendingEntry(const BlockPos &_pos, std::unique_ptr<BaseCircuitComponent> _component)
{
    this->mComponent = std::move(_component);
    this->mRawComponentPtr = nullptr;
    this->mPos = _pos;
}

CircuitSceneGraph::PendingEntry::PendingEntry(const BlockPos &_pos, BaseCircuitComponent *_component)
{
    this->mRawComponentPtr = _component;
    this->mComponent = nullptr;
    this->mPos = _pos;
}

CircuitSceneGraph::PendingEntry &CircuitSceneGraph::PendingEntry::operator=(CircuitSceneGraph::PendingEntry &&rhs)
{
    this->mComponent = std::move(rhs.mComponent);
    this->mRawComponentPtr = rhs.mRawComponentPtr;
    this->mPos = rhs.mPos;
    return *this;
}
