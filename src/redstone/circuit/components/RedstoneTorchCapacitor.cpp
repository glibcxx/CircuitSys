#include "RedstoneTorchCapacitor.h"

#include "redstone/circuit/CircuitSystem.h"
#include "Facing.h"

bool isQueueBeingPowered(CircuitComponentList &sources);

bool findRepeaterScenario(CircuitComponentList &sources, CircuitSceneGraph &system);

void PushCircularReference(std::unordered_map<BlockPos, RedstoneTorchCapacitor *> &relatedTorches, const BlockPos &pos, std::queue<RedstoneTorchCapacitor *> &list);

int RedstoneTorchCapacitor::getStrength() const
{
    return this->mState[0].mOn ? Redstone::SIGNAL_MAX : Redstone::SIGNAL_NONE;
}

void RedstoneTorchCapacitor::removeSource(const BlockPos &posSource, const BaseCircuitComponent *pComponent)
{
    this->BaseCircuitComponent::removeSource(posSource, pComponent);
    if (this->mNextOrder == pComponent && this->mNextOrder)
        this->mNextOrder = this->mNextOrder->mNextOrder;
}

bool RedstoneTorchCapacitor::addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered)
{
    CircuitComponentType id = info.mNearest.mTypeID;
    int                  direction = info.mCurrent.mDirection;
    if (id != CircuitComponentType::PoweredBlockComponent
        && id != CircuitComponentType::TransporterComponent
        && id != CircuitComponentType::ConsumerComponent
        && direction == Facing::OPPOSITE_FACING[info.mCurrent.mComponent->getDirection()])
    {
        this->BaseCircuitComponent::trackPowerSource(info, dampening, bDirectlyPowered, 0);
    }
    return false;
}

bool RedstoneTorchCapacitor::allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    bool                bAttached = this->getDirection() == info.mCurrent.mDirection;
    CircuitTrackingInfo associatedInfo(info);
    associatedInfo.mPower.mPos = info.mCurrent.mPos;
    associatedInfo.mPower.mComponent = info.mCurrent.mComponent;
    this->trackPowerSource(associatedInfo, info.mDampening, info.mDirectlyPowered, 0);
    if (((info.mCurrent.mTypeID == CircuitComponentType::PoweredBlockComponent || info.mCurrent.mTypeID == CircuitComponentType::ProducerComponent)
             && info.mCurrent.mDirection == info.mNearest.mDirection
         || info.mCurrent.mTypeID == CircuitComponentType::ConsumerComponent && bAttached)
        && (info.mCurrent.mTypeID == CircuitComponentType::PoweredBlockComponent))
    {
        return false;
    }
    else if (!info.mCurrent.mDirection)
        return this->mDirection != 0;
    else
        return !(this->mDirection && this->mDirection == 0);
}

bool RedstoneTorchCapacitor::evaluate(CircuitSystem &system, const BlockPos &pos)
{
    this->mState[0] = this->mState[1];
    if (this->mNextOrder && this->_canIncrementSelfPower())
        ++this->mSelfPowerCount;
    return this->mState[0].mChanged;
}

void RedstoneTorchCapacitor::cacheValues(CircuitSystem &system, const BlockPos &pos)
{
    if (this->mNextOrder)
    {
        this->mState[1].mHalfFrame = false;
        if (isQueueBeingPowered(this->mSources))
        {
            this->mState[1].mOn = true;
            this->mState[1].mChanged = !this->mState[0].mOn;
            this->mSelfPowerCount = -1;
        }
        else if (this->mSelfPowerCount)
        {
            this->mState[1].mOn = false;
            this->mState[1].mChanged = this->mState[0].mOn;
        }
        else
        {
            this->mState[1].mOn = true;
            this->mState[1].mChanged = !this->mState[0].mOn;
            this->mNextOrder->mSelfPowerCount = -1;
            RedstoneTorchCapacitor *others = this->mNextOrder->mNextOrder;
            for (int count = 0; count < 4 && others != nullptr && others != this; ++count)
            {
                others->mSelfPowerCount = 1;
                others = others->mNextOrder;
            }
        }
    }
    else
    {
        bool bSelfPowered = 0;
        bool bHalfFrame;
        bool onState;
        int  newStrength = this->_findStrongestStrength(pos, system, bSelfPowered);

        if (bSelfPowered)
        {
            if (this->_canIncrementSelfPower())
                ++this->mSelfPowerCount;

            if (this->mSelfPowerCount >= 16)
            {
                onState = 0;
                bHalfFrame = this->mSelfPowerCount == 16;
            }
            else
            {
                onState = !this->mState[0].mOn;
                bHalfFrame = 1;
            }
        }
        else
        {
            this->mSelfPowerCount = 0;
            this->mCanReigniteFromBurnout = 0;
            onState = newStrength <= 0;
            bHalfFrame = 0;
        }
        bool bChange = onState != this->mState[0].mOn;
        this->mState[1].mHalfFrame = bHalfFrame;
        this->mState[1].mOn = onState;
        this->mState[1].mChanged = bChange;
        if (this->mSelfPowerCount > 32)
            this->mCanReigniteFromBurnout = 1;
    }
}

void RedstoneTorchCapacitor::updateDependencies(CircuitSceneGraph &system, const BlockPos &pos)
{
    if (!findRepeaterScenario(this->mSources, system))
        this->mNextOrder = nullptr;
}

bool RedstoneTorchCapacitor::isHalfPulse()
{
    return this->mState[0].mHalfFrame;
}

int RedstoneTorchCapacitor::getSelfPowerCount()
{
    return this->mSelfPowerCount;
}

int RedstoneTorchCapacitor::_findStrongestStrength(const BlockPos &targetPos, CircuitSystem &system, bool &bPowerFromSelf)
{
    bPowerFromSelf = 0;
    int newStrength = 0;
    if (this->mSources.size() > 0)
    {
        CircuitComponentList::Item *pFoundEntry = nullptr;
        CircuitComponentList::Item *pSelfEntry = nullptr;
        CircuitComponentList::Item *pNotSelfEntry = nullptr;
        for (auto &entry : this->mSources)
        {
            if (!entry.mComponent)
                continue;

            CircuitComponentType searchID = entry.mComponent->getCircuitComponentGroupType();
            if (searchID == CircuitComponentType::ProducerComponent
                || searchID == CircuitComponentType::CapacitorComponent && !entry.mComponent->mSources.size())
            {
                int curStrength = entry.mComponent->getStrength();
                if (curStrength >= newStrength && curStrength)
                {
                    pFoundEntry = &entry;
                    newStrength = curStrength;
                }
            }
            else
            {
                int indirectValue = 0;
                int selfValue = 0;
                for (auto searchEntry : entry.mComponent->mSources)
                {
                    if (searchEntry.mDirectlyPowered && searchEntry.mComponent)
                    {
                        indirectValue = std::max(searchEntry.mComponent->getStrength() - searchEntry.mDampening, 0);
                        if (indirectValue >= newStrength && indirectValue != 0)
                        {
                            newStrength = indirectValue;
                            pFoundEntry = &searchEntry;
                            if (targetPos != searchEntry.mPos)
                                pNotSelfEntry = &searchEntry;
                        }
                        if (targetPos == searchEntry.mPos)
                        {
                            pSelfEntry = &searchEntry;
                            selfValue = indirectValue;
                        }
                    }
                }

                if (indirectValue == selfValue && pNotSelfEntry)
                {
                    pFoundEntry = pNotSelfEntry;
                    pSelfEntry = nullptr;
                }
                else if (!newStrength && pSelfEntry)
                {
                    pFoundEntry = pSelfEntry;
                }
                bPowerFromSelf = pFoundEntry == pSelfEntry && pSelfEntry != nullptr;
            }
        }
    }
    return newStrength;
}

bool RedstoneTorchCapacitor::_canIncrementSelfPower()
{
    return !this->mCanReigniteFromBurnout && this->mSelfPowerCount <= 32;
}

void RedstoneTorchCapacitor::setOn(bool bOn)
{
    this->mState[0].mOn = bOn;
    this->mState[0].mChanged = true;
    this->mState[0].mHalfFrame = false;
    this->mSelfPowerCount = false;
    this->mCanReigniteFromBurnout = false;
    this->mNextOrder = nullptr;
}

void RedstoneTorchCapacitor::resetBurnOutCount()
{
    if (this->mCanReigniteFromBurnout)
    {
        this->mSelfPowerCount = 0;
        this->mCanReigniteFromBurnout = 0;
    }
}

void RedstoneTorchCapacitor::setSelfPowerCount(int count)
{
    this->mSelfPowerCount = count;
}

void RedstoneTorchCapacitor::setNextInQueue(RedstoneTorchCapacitor *torch)
{
    this->mNextOrder = torch;
}

bool isQueueBeingPowered(CircuitComponentList &sources)
{
    if (sources.size() == 0)
    {
        return false;
    }
    for (auto &item : sources)
    {
        if (!item.mComponent)
        {
            return false;
        }
        CircuitComponentType searchID = item.mComponent->getCircuitComponentGroupType();
        if (searchID == CircuitComponentType::PoweredBlockComponent || searchID == CircuitComponentType::ConsumerComponent)
        {
            for (auto &source : item.mComponent->mSources)
            {
                auto type = source.mComponent->getCircuitComponentType();
                if (source.mDirectlyPowered
                    && source.mComponent
                    && type != CircuitComponentType::RedstoneTorchCapacitor
                    && std::max(source.mComponent->getStrength() - source.mDampening, 0))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool findRepeaterScenario(CircuitComponentList &sources, CircuitSceneGraph &system)
{
    bool bFound = false;
    for (auto &entry : sources)
    {
        if (!entry.mComponent)
        {
            return false;
        }
        BlockPos             targetUpPos = entry.mPos + BlockPos(0, 1, 0);
        CircuitComponentType searchID = entry.mComponent->getCircuitComponentGroupType();

        if (system.getComponent(targetUpPos, CircuitComponentType::TransporterComponent)
            && (searchID == CircuitComponentType::PoweredBlockComponent || searchID == CircuitComponentType::ConsumerComponent))
        {
            std::unordered_map<BlockPos, RedstoneTorchCapacitor *> relatedTorches;
            for (auto &searchEntry : entry.mComponent->mSources)
            {
                if (searchEntry.mDirectlyPowered && searchEntry.mComponent)
                {
                    auto     direction = searchEntry.mComponent->getDirection();
                    BlockPos powerBlockTarget = searchEntry.mPos + Facing::DIRECTION[direction];
                    if (powerBlockTarget == entry.mPos && searchEntry.mComponent->getCircuitComponentType() == CircuitComponentType::RedstoneTorchCapacitor)
                    {
                        BlockPos                searchUpPos = searchEntry.mPos + BlockPos(0, 1, 0);
                        RedstoneTorchCapacitor *torch = (RedstoneTorchCapacitor *)searchEntry.mComponent;

                        if ((system.getComponent(searchUpPos, CircuitComponentType::PoweredBlockComponent) || system.getComponent(searchUpPos, CircuitComponentType::ConsumerComponent)) && torch->getDirection())
                        {
                            relatedTorches.insert({searchEntry.mPos, torch});
                        }
                    }
                }
            }
            if (relatedTorches.size() > 1)
            {
                std::queue<RedstoneTorchCapacitor *> list;

                BlockPos northPos = entry.mPos + BlockPos(0, 0, -1);
                BlockPos westPos = entry.mPos + BlockPos(-1, 0, 0);
                BlockPos southPos = entry.mPos + BlockPos(0, 0, 1);
                BlockPos eastPos = entry.mPos + BlockPos(1, 0, 0);
                PushCircularReference(relatedTorches, northPos, list);
                PushCircularReference(relatedTorches, westPos, list);
                PushCircularReference(relatedTorches, southPos, list);
                PushCircularReference(relatedTorches, eastPos, list);

                if (list.size())
                {
                    RedstoneTorchCapacitor *front = list.front();
                    front->setSelfPowerCount(0);
                    RedstoneTorchCapacitor *back = list.back();
                    back->setNextInQueue(front);
                    back->setSelfPowerCount(0);
                    while (list.size() > 1)
                    {
                        RedstoneTorchCapacitor *current = list.front();
                        list.pop();
                        RedstoneTorchCapacitor *next = list.front();
                        current->setNextInQueue(next);
                        back->setSelfPowerCount(16);
                    }
                }
                bFound = true;
            }
        }
    }
    return bFound;
}

void PushCircularReference(std::unordered_map<BlockPos, RedstoneTorchCapacitor *> &relatedTorches,
                           const BlockPos                                         &pos,
                           std::queue<RedstoneTorchCapacitor *>                   &list)
{
    auto iteration = relatedTorches.find(pos);
    if (iteration != relatedTorches.end())
    {
        list.push(iteration->second);
    }
}