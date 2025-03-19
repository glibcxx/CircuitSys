#include "ComparatorCapacitor.h"

CircuitComponentType ComparatorCapacitor::getCircuitComponentType() const
{
    return this->mCircuitComponentType;
}

bool ComparatorCapacitor::allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    return info.mCurrent.mDirection == this->getPoweroutDirection();
}

bool ComparatorCapacitor::addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered)
{
    FacingID searchDir = info.mCurrent.mDirection;
    if (searchDir == Facing::DOWN || searchDir == Facing::UP)
        return false;
    if (searchDir == this->getDirection())
        return false;

    CircuitComponentType instanceID = info.mNearest.mComponent->getCircuitComponentType();
    if (searchDir == Facing::OPPOSITE_FACING[this->getDirection()]
        || instanceID == CircuitComponentType::TransporterComponent
        || instanceID == CircuitComponentType::RepeaterCapacitor
        || instanceID == CircuitComponentType::ComparatorCapacitor)
    {
        if (info.mNearest.mTypeID != CircuitComponentType::PoweredBlockComponent || info.mNearest.mComponent->hasDirectPower())
        {
            this->trackPowerSource(info, dampening, bDirectlyPowered, 0, true);
        }
    }
    return false;
}

bool ComparatorCapacitor::evaluate(CircuitSystem &system, const BlockPos &pos)
{
    this->mOldStrength = this->mStrength;
    if (this->mMode == Mode::SUBTRACT_MODE)
    {
        int strength = this->GetRearStrength();
        this->mStrength = std::max(strength - this->GetSideStrength(), 0);
    }
    else
    {
        this->mStrength = this->GetRearStrength();
        if (this->GetSideStrength() > this->mStrength)
            this->mStrength = 0;
    }
    this->mStrength = std::min(this->mStrength, Redstone::SIGNAL_MAX);
    return this->mOldStrength != this->mStrength;
}

int ComparatorCapacitor::GetRearStrength()
{
    return this->mRearAnalogStrength == -1 ? this->mRearStrength : this->mRearAnalogStrength;
}

int ComparatorCapacitor::GetSideStrength()
{
    return (this->mSideAnalogStrengthRight == -1 && this->mSideAnalogStrengthLeft == -1)
             ? this->mSideStrengths
             : std::max(this->mSideAnalogStrengthRight, this->mSideAnalogStrengthLeft);
}

void ComparatorCapacitor::cacheValues(CircuitSystem &system, const BlockPos &pos)
{
    if (this->mHasAnalogBeenSet)
    {
        this->mRearStrength = 0;
        this->mSideStrengths = 0;
        for (auto &&iter : this->mSources)
        {
            if (iter.mDirectlyPowered)
            {
                int currentStrength = std::max(iter.mComponent->getStrength() - iter.mDampening, 0);
                if (currentStrength > this->mRearStrength)
                    this->mRearStrength = currentStrength;
            }
        }
        for (auto &&iter : this->mSideComponents.mComponents)
        {
            int currentStrength = std::max(iter.mComponent->getStrength() - iter.mDampening, 0);
            if (currentStrength > this->mSideStrengths)
                this->mSideStrengths = currentStrength;
        }
    }
}

void ComparatorCapacitor::setAnalogStrength(int strength, FacingID dir)
{
    if (dir == this->getDirection())
    {
        this->mRearAnalogStrength = std::max(strength, 0);
    }
    else if (dir != Facing::OPPOSITE_FACING[this->getDirection()])
    {
        if (dir == Facing::getClockWise(dir))
        {
            this->mSideAnalogStrengthLeft = std::max(strength, 0);
        }
        else
        {
            this->mSideAnalogStrengthRight = std::max(strength, 0);
        }
    }
    this->mHasAnalogBeenSet = 1;
}

void ComparatorCapacitor::clearAnalogStrength(FacingID dir)
{
    if (dir == this->getDirection())
    {
        this->mRearAnalogStrength = -1;
    }
    else if (dir != Facing::OPPOSITE_FACING[this->getDirection()])
    {
        if (dir == Facing::getClockWise(dir))
            this->mSideAnalogStrengthLeft = -1;
        else
            this->mSideAnalogStrengthRight = -1;
    }
    this->mHasAnalogBeenSet = 1;
}

FacingID ComparatorCapacitor::getPoweroutDirection() const
{
    return Facing::OPPOSITE_FACING[this->getDirection()];
}

void ComparatorCapacitor::updateDependencies(CircuitSceneGraph &system, const BlockPos &pos)
{
    for (auto iter = this->mSources.begin(); iter != this->mSources.end();)
    {
        CircuitComponentList::Item &systemComponent = *iter;
        if (systemComponent.mComponent->canStopPower())
        {
            systemComponent.mComponent->mDestinationList.erase(this);
            iter = this->mSources.erase(iter);
        }
        else if (iter->mDirection == Facing::OPPOSITE_FACING[this->getDirection()])
        {
            iter++;
        }
        else
        {
            this->mSideComponents.add(iter->mComponent, iter->mDampening, iter->mPos);
            iter = this->mSources.erase(iter);
        }
    }
}

void ComparatorCapacitor::setCachedStrength(int iStrength)
{
    this->mOldStrength = iStrength;
    this->mRearAnalogStrength = iStrength;
    this->setStrength(iStrength);
}
