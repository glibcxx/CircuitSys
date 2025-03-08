#include "TransporterComponent.h"

#include "redstone/circuit/CircuitSceneGraph.h"
#include "PoweredBlockComponent.h"
#include "ConsumerComponent.h"
#include "RepeaterCapacitor.h"

bool isPowerFlowAvailabeAtPos(CircuitSceneGraph &graph, const BlockPos &currentPos, FacingID dir, bool sameLevel)
{
    BaseCircuitComponent *component = graph.getBaseComponent(currentPos);
    if (!component)
    {
        return false;
    }
    uint64_t typeID = component->getBaseType();
    if (component->getInstanceType() == CS_Repeater)
    {
        FacingID facing = static_cast<RepeaterCapacitor *>(component)->getPoweroutDirection();
        return (dir == facing || dir == Facing::OPPOSITE_FACING[facing]) && sameLevel;
    }
    if (component->getInstanceType() == CS_Pulse)
    {
        return dir == Facing::OPPOSITE_FACING[static_cast<RepeaterCapacitor *>(component)->getPoweroutDirection()] && sameLevel;
    }
    else if (typeID == CS_Transporter)
    {
        return true;
    }
    else if (typeID == CS_Producer || typeID == CS_Capacitor || typeID == CS_Piston)
    {
        return sameLevel;
    }
    else
    {
        return false;
    }
}

bool isDirectPowerByPowerBlock(const CircuitTrackingInfo &info)
{
    if (info.m2ndNearest.mTypeID == CS_PoweredBlock)
        return static_cast<PoweredBlockComponent *>(info.m2ndNearest.mComponent)->isPromotedToProducer();
    if (info.m2ndNearest.mTypeID == CS_Consumer)
        return static_cast<ConsumerComponent *>(info.m2ndNearest.mComponent)->isPromotedToProducer();
    return info.m2ndNearest.mTypeID == CS_Producer || info.m2ndNearest.mComponent->getInstanceType() == CS_Torch;
}

bool isBlockAboveStopingPower(CircuitSceneGraph &graph, const BlockPos &currentPos)
{
    BlockPos              pos = currentPos + Facing::DIRECTION[1];
    BaseCircuitComponent *up = graph.getBaseComponent(pos);
    return up && !up->canAllowPowerUp();
}

bool isBlockBelowStopingPower(CircuitSceneGraph &graph, const BlockPos &currentPos)
{
    BlockPos              pos = currentPos + Facing::DIRECTION[0];
    BaseCircuitComponent *down = graph.getBaseComponent(pos);
    return down && !down->canAllowPowerDown();
}

bool isPowerFlowAvailabeAtDir(CircuitSceneGraph &graph, const BlockPos &currentPos, FacingID dir)
{
    BlockPos targetPos = currentPos + Facing::DIRECTION[dir];
    BlockPos targetUpPos = targetPos + Facing::DIRECTION[1];
    BlockPos targetDownPos = targetPos + Facing::DIRECTION[0];
    bool     isWireDown = isPowerFlowAvailabeAtPos(graph, targetDownPos, dir, false) && !isBlockAboveStopingPower(graph, targetDownPos);
    bool     isWireUp = isPowerFlowAvailabeAtPos(graph, targetUpPos, dir, false) && !isBlockAboveStopingPower(graph, currentPos);
    return isPowerFlowAvailabeAtPos(graph, targetPos, dir, true) || isWireUp || isWireDown;
}

bool IsDotOrDirectLine(CircuitSceneGraph &graph, const BlockPos &nearestPos, const BlockPos &deltaIn)
{
    int xCount = 0;
    int zCount = 0;
    int flowCount = 0;
    if (isPowerFlowAvailabeAtDir(graph, nearestPos, Facing::NORTH))
    {
        if (deltaIn.z)
            ++zCount;
        ++flowCount;
    }
    if (isPowerFlowAvailabeAtDir(graph, nearestPos, Facing::SOUTH))
    {
        if (deltaIn.z)
            ++zCount;
        ++flowCount;
    }
    if (isPowerFlowAvailabeAtDir(graph, nearestPos, Facing::EAST))
    {
        if (deltaIn.x)
            ++xCount;
        ++flowCount;
    }
    if (isPowerFlowAvailabeAtDir(graph, nearestPos, Facing::WEST))
    {
        if (deltaIn.x)
            ++xCount;
        ++flowCount;
    }
    return flowCount == 0 || (flowCount == 1 && (zCount == 1 || xCount == 1));
}

bool TransporterComponent::addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered)
{
    uint64_t type = info.mNearest.mTypeID;
    if (type == CS_PoweredBlock && !bDirectlyPowered)
        return false;

    if (type == CS_Producer || type == CS_Transporter || type == CS_Capacitor)
        bDirectlyPowered = true;

    else if (type == CS_PoweredBlock && static_cast<PoweredBlockComponent *>(info.mNearest.mComponent)->isPromotedToProducer())
        bDirectlyPowered = true;

    else if (type == CS_Consumer && static_cast<ConsumerComponent *>(info.mNearest.mComponent)->isPromotedToProducer())
        bDirectlyPowered = true;

    else if (info.mCurrent.mDirection != Facing::DOWN && info.mCurrent.mDirection != Facing::OPPOSITE_FACING[info.mNearest.mDirection])
        bDirectlyPowered = info.mCurrent.mDirection == info.mNearest.mDirection;
    else
        bDirectlyPowered = true;

    return this->trackPowerSource(info, ++dampening, bDirectlyPowered, 0);
}

bool TransporterComponent::allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    if (info.mCurrent.mDirection == Facing::UP)
        return false;

    if (info.mCurrent.mDirection == Facing::DOWN || info.mCurrent.mComponent->consumePowerAnyDirection())
        return true;

    if (info.mCurrent.mTypeID == CS_Transporter)
    {
        BlockPos deltaIn = info.mCurrent.mPos - info.mNearest.mPos;
        if (deltaIn.y == 0)
        {
            return true;
        }
        else if (deltaIn.y > 0)
        {
            if (isBlockAboveStopingPower(graph, info.mNearest.mPos))
            {
                return false;
            }
        }
        else
        {
            if (isBlockAboveStopingPower(graph, info.mCurrent.mPos) || isBlockBelowStopingPower(graph, info.mNearest.mPos))
            {
                return false;
            }
        }
    }

    BlockPos        deltaIn = info.m2ndNearest.mPos - info.mCurrent.mPos;
    const BlockPos &nearestPos = info.mNearest.mPos;

    bool bFlowPossible = false;
    if (deltaIn.z == -2 || deltaIn.z == 2)
    {
        bFlowPossible = !isPowerFlowAvailabeAtDir(graph, nearestPos, Facing::EAST)
                     && !isPowerFlowAvailabeAtDir(graph, nearestPos, Facing::WEST);
    }
    else if (deltaIn.x == -2 || deltaIn.x == 2)
    {
        bFlowPossible = !isPowerFlowAvailabeAtDir(graph, nearestPos, Facing::NORTH)
                     && !isPowerFlowAvailabeAtDir(graph, nearestPos, Facing::SOUTH);
    }
    else
    {
        if ((deltaIn.y == 1 || deltaIn.y == -1) && (deltaIn.x || deltaIn.z))
        {
            BlockPos posDelta = info.mNearest.mPos - info.mCurrent.mPos;
            bDirectlyPowered = IsDotOrDirectLine(graph, nearestPos, posDelta);
            return bDirectlyPowered;
        }
        if ((info.mCurrent.mTypeID == CS_PoweredBlock || info.mCurrent.mTypeID == CS_Consumer)
            && deltaIn.y == 0 && deltaIn.x && deltaIn.z)
        {
            BlockPos posDelta = info.mNearest.mPos - info.mCurrent.mPos;
            bDirectlyPowered = IsDotOrDirectLine(graph, nearestPos, posDelta);
            return bDirectlyPowered;
        }
        if (isDirectPowerByPowerBlock(info))
        {
            bFlowPossible = !isPowerFlowAvailabeAtDir(graph, nearestPos, Facing::NORTH)
                         && !isPowerFlowAvailabeAtDir(graph, nearestPos, Facing::SOUTH)
                         && !isPowerFlowAvailabeAtDir(graph, nearestPos, Facing::WEST)
                         && !isPowerFlowAvailabeAtDir(graph, nearestPos, Facing::EAST);
            if (bFlowPossible)
                bDirectlyPowered = true;
        }
    }
    return bFlowPossible;
}

void TransporterComponent::cacheValues(CircuitSystem &, const BlockPos &pos)
{
    int currentStrength = 0, newStrength = 0;
    for (auto &item : this->mSources.mComponents)
    {
        currentStrength = item.mComponent->getStrength() - item.mDampening;
        if (currentStrength < 0)
        {
            currentStrength = 0;
        }
        if (newStrength < currentStrength)
        {
            newStrength = currentStrength;
        }
    }
    this->mNextStrength = newStrength;
}

bool TransporterComponent::canConsumerPower()
{
    return true;
}

bool TransporterComponent::evaluate(CircuitSystem &, const BlockPos &)
{
    bool changed = this->mStrength != this->mNextStrength;
    this->mStrength = this->mNextStrength;
    return changed;
}

uint64_t TransporterComponent::getBaseType() const
{
    return CS_Transporter;
}

uint64_t TransporterComponent::getInstanceType() const
{
    return CS_Transporter;
}
