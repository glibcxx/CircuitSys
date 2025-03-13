#include "RepeaterCapacitor.h"

#include "deps/Math.h"

void RepeaterCapacitor::delayPulse(RepeaterCapacitor::States States)
{
    for (int i = 0; i < this->mInsertAt; ++i)
    {
        this->mOnStates[i] = this->mOnStates[i + 1];
    }
    for (int i = mInsertAt; i < 4; ++i)
    {
        this->mOnStates[i] = States;
    }
}

int RepeaterCapacitor::getStrength() const
{
    return this->mPowered ? Redstone::SIGNAL_MAX : Redstone::SIGNAL_NONE;
}

void RepeaterCapacitor::setDelay(int delay)
{
    this->mInsertAt = mce::Math::clamp(delay, 0, 3);
}

bool RepeaterCapacitor::evaluate(CircuitSystem &system, const BlockPos &pos)
{
    auto oldPowered = this->mPowered;
    if (this->mLocked)
    {
        return false;
    }
    this->delayPulse((RepeaterCapacitor::States)this->mNextPower);
    if (this->mInsertAt > 0)
    {
        if (this->mPulseCount > this->mInsertAt && (this->mPulse == 0 || this->mPulseCount != 2))
        {
            this->alternatePulse();
        }
        else
        {
            this->extendPulse();
        }
    }

    this->mPowered = this->mOnStates[0] == States::ON || this->mOnStates[0] == States::ON_LOCKED;
    return oldPowered != this->mPowered;
}

void RepeaterCapacitor::alternatePulse()
{
    States curState = States::OFF;
    if (this->mInsertAt > 0)
    {
        curState = this->mOnStates[0];
        if (curState == States::OFF || curState == States::ON)
        {
            States newStates = this->mNextPulse ? States::ON_LOCKED : States::OFF_LOCKED;
            this->mNextPulse = !this->mNextPulse;
            for (int i = 0; i < this->mInsertAt + 1; ++i)
            {
                this->mOnStates[i] = newStates;
            }
        }
    }
}

void RepeaterCapacitor::extendPulse()
{
    if (this->mInsertAt > 0)
    {
        if (this->mOnStates[0] == States::OFF_LOCKED && this->mOnStates[1] == States::OFF)
        {
            this->mOnStates[1] = States::OFF_LOCKED;
        }
        else if (this->mOnStates[0] == States::ON_LOCKED && this->mOnStates[1] == States::ON)
        {
            this->mOnStates[1] = States::ON_LOCKED;
        }

        States curState = this->mOnStates[0];
        if (curState == States::OFF || curState == States::ON)
        {
            int iPulse = 0;
            for (int i = 0; i <= this->mInsertAt; ++i)
            {
                if (this->mOnStates[i] == curState)
                {
                    ++iPulse;
                }
            }

            States value;
            if (curState == States::ON)
            {
                value = States::ON_LOCKED;
            }
            else
            {
                value = iPulse <= this->mInsertAt ? States::ON_LOCKED : States::OFF_LOCKED;
            }
            for (int i = 0; i <= this->mInsertAt; ++i)
            {
                this->mOnStates[i] = value;
            }
        }
    }
}

inline FacingID RepeaterCapacitor::getPoweroutDirection() const
{
    return Facing::OPPOSITE_FACING[this->getDirection()];
}

bool RepeaterCapacitor::allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    return info.mCurrent.mDirection == this->getPoweroutDirection();
}

bool RepeaterCapacitor::addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered)
{
    bool forward = this->getDirection() == info.mCurrent.mDirection;
    bool backward = Facing::OPPOSITE_FACING[this->getDirection()] == info.mCurrent.mDirection;
    if (forward)
        return false;

    if (info.mNearest.mTypeID == CircuitComponentType::PoweredBlockComponent && (!info.mNearest.mComponent->hasDirectPower() || !bDirectlyPowered))
        return false;
    auto type = info.mNearest.mComponent->getCircuitComponentType();
    if (backward || type == CircuitComponentType::RepeaterCapacitor || type == CircuitComponentType::ComparatorCapacitor)
    {
        this->trackPowerSource(info, dampening, bDirectlyPowered, backward);
    }

    return false;
}

void RepeaterCapacitor::updateDependencies(CircuitSceneGraph &system, const BlockPos &pos)
{
    for (auto iter = this->mSources.begin(); iter != this->mSources.end();)
    {
        CircuitComponentList::Item &systemComponent = *iter;
        if (iter->mData == 1)
        {
            iter++;
        }
        else
        {
            this->mSideComponents.add(systemComponent.mComponent, iter->mDampening, iter->mPos);
            iter = this->mSources.erase(iter);
        }
    }
}

void RepeaterCapacitor::setStrength(int strength)
{
    this->mPowered = strength != 0;
    for (int i = 0; i < this->mInsertAt + 1; ++i)
    {
        this->mOnStates[i] = this->mPowered ? States::ON_LOCKED : States::OFF_LOCKED;
    }
    for (int i = this->mInsertAt + 1; i <= 4; ++i)
    {
        this->mOnStates[i] = States::OFF;
    }
}

void RepeaterCapacitor::checkLock(CircuitSystem &system, const BlockPos &pos)
{
    this->mLocked = false;
    for (auto &item : this->mSideComponents)
    {
        int currentStrength = std::max(item.mComponent->getStrength() - item.mDampening, 0);
        if (currentStrength > 0)
        {
            this->mLocked = true;
            for (int idx = 1; idx < 4; ++idx)
            {
                this->mOnStates[idx] = this->mOnStates[0];
            }
        }
    }
}

void RepeaterCapacitor::cacheValues(CircuitSystem &system, const BlockPos &pos)
{
    this->mNextPower = 0;
    for (auto &item : this->mSources)
    {
        int inputPower = item.mComponent->getStrength() - item.mDampening;
        inputPower = std::max(inputPower, 0);
        if (inputPower > 0)
        {
            this->mNextPower = 1;
            break;
        }
    }

    if (!this->mLocked)
    {
        if (this->mPulse == this->mNextPower)
        {
            this->mPulseCount = 0;
        }
        else
        {
            this->mPulseCount++;
            if (this->mPulseCount == 1)
            {
                this->mNextPulse = !this->mPulse;
            }
            this->mPulse = !this->mPulse;
        }
    }
}