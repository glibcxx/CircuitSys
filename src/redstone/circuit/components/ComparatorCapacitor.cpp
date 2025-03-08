#include "ComparatorCapacitor.h"

uint64_t ComparatorCapacitor::getInstanceType() const
{
    return CS_Comparator;
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
    if (searchDir != this->getDirection())
        return false;
    // 只有从主输入端、侧输入端方向来的才有可能成为信号源

    uint64_t instanceID = info.mNearest.mComponent->getInstanceType();
    if (searchDir == Facing::OPPOSITE_FACING[this->getDirection()]
        || instanceID == CS_Transporter
        || instanceID == CS_Repeater
        || instanceID == CS_Comparator) // 侧输入端只能为红石粉、中继器、比较器
    {
        if (info.mNearest.mTypeID != CS_PoweredBlock || info.mNearest.mComponent->hasDirectPower())
        {
            return this->trackPowerSourceDuplicates(info, dampening, bDirectlyPowered);
        }
    }
    return false;
}

bool ComparatorCapacitor::consumePowerAnyDirection()
{
    return true;
}

bool ComparatorCapacitor::canConsumerPower()
{
    return true;
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
    return (this->mSideAnalogStrengthRight == -1 && this->mSideAnalogStrengthLeft == -1) ? this->mSideStrengths : std::max(this->mSideAnalogStrengthRight, this->mSideAnalogStrengthLeft);
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

bool ComparatorCapacitor::isSubtractMode()
{
    return this->mMode == Mode::SUBTRACT_MODE;
}

void ComparatorCapacitor::setMode(ComparatorCapacitor::Mode mode)
{
    this->mMode = mode;
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

inline int ComparatorCapacitor::getOldStrength()
{
    return this->mOldStrength;
}

/**
 * @brief 将mSources内属于侧输入的，移至mSideComponents内
 * @param system 电路图
 * @param pos 比较器位置
 */
void ComparatorCapacitor::updateDependencies(CircuitSceneGraph &system, const BlockPos &pos)
{
    for (auto iter = this->mSources.begin(); iter != this->mSources.end();)
    {
        CircuitComponentList::Item &systemComponent = *iter;
        if (systemComponent.mComponent->canStopPower())
        {
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

bool ComparatorCapacitor::removeSource(const BlockPos &posSource, const BaseCircuitComponent *pComponent)
{
    bool bFound = this->removeSource(posSource, pComponent);
    return this->mSideComponents.removeSource(posSource, pComponent) || bFound;
}

void ComparatorCapacitor::setCachedStrength(int iStrength)
{
    this->mOldStrength = iStrength;
    this->mRearAnalogStrength = iStrength;
    this->setStrength(iStrength);
}
