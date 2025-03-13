#include "BaseRailTransporter.h"
#include "Facing.h"

bool insertIntoEntries(BaseRailTransporter *trasnporter, const CircuitTrackingInfo &info, int dampening, int distance)
{
    int newDampening = dampening - 1 > 0 ? dampening - 1 : 0;
    for (auto &&source : trasnporter->mSources)
    {
        if (source.mPos == info.mPower.mPos && source.mData == distance - 1 && source.mDampening == newDampening)
        {
            return false;
        }
    }
    CircuitComponentList::Item value;
    value.mDampening = newDampening;
    value.mPos = info.mPower.mPos;
    value.mDirection = info.mCurrent.mDirection;
    value.mDirectlyPowered = true;
    value.mComponent = info.mPower.mComponent;
    value.mData = distance - 1 >= 0 ? distance - 1 : 0;
    trasnporter->addSourceItem(value);
    return true;
}

CircuitComponentType BaseRailTransporter::getCircuitComponentType() const
{
    return this->mCircuitComponentType;
}

bool BaseRailTransporter::addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered)
{
    if (!bDirectlyPowered)
        return false;

    BaseCircuitComponent *current = info.mNearest.mComponent;
    CircuitComponentType  id = current->getCircuitComponentType();
    if (id == CircuitComponentType::BaseRailTransporter)
    {
        CircuitTrackingInfo newInfo{info};
        int                 distance = 0;
        int                 nSources = this->mSources.size();
        for (auto &entry : current->mSources)
        {
            if (entry.mData - 1 > 0)
            {
                newInfo.mPower.mPos = entry.mPos;
                newInfo.mPower.mDirection = this->mDirection;
                newInfo.mDampening = entry.mDampening;
                newInfo.mData = entry.mData;
                insertIntoEntries(this, newInfo, newInfo.mDampening + 1, newInfo.mData);
                if (newInfo.mData > distance)
                    distance = newInfo.mData;
            }
        }
        return this->mSources.size() != nSources;
    }
    else
    {
        return insertIntoEntries(this, info, dampening, 10);
    }
}

bool BaseRailTransporter::allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    if (info.mCurrent.mComponent->getCircuitComponentType() == CircuitComponentType::BaseRailTransporter
        && this->mRailType == ((BaseRailTransporter *)info.mCurrent.mComponent)->mRailType)
    {
        if (info.mCurrent.mComponent->getDirection() == this->mDirection)
        {
            if (info.mCurrent.mDirection != this->mDirection)
                return Facing::OPPOSITE_FACING[this->mDirection] == info.mCurrent.mDirection;
            return true;
        }
    }
    return false;
}

bool BaseRailTransporter::evaluate(CircuitSystem &system, const BlockPos &pos)
{
    int bPrevStrength = this->mStrength;
    this->mStrength = 0;
    for (auto &source : this->mSources)
    {
        int distance = source.mData;
        if (distance > this->mStrength && source.mComponent && (source.mComponent->getStrength() - source.mDampening) > 0)
        {
            this->mStrength = distance;
        }
    }
    return this->mStrength != bPrevStrength;
}
