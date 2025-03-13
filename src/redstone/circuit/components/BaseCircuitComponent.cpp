#include "BaseCircuitComponent.h"
#include "redstone/circuit/CircuitSystem.h"
#include "redstone/circuit/CircuitTrackingInfo.h"
#include "deps/Math.h"

bool BaseCircuitComponent::calculateValue(CircuitSystem &system)
{
    int newStrength = 0;

    for (auto &source : this->mSources.mComponents)
    {
        auto currentStrength = source.mComponent->getStrength() - source.mDampening;

        if (source.mComponent->isHalfPulse())
            currentStrength = Redstone::SIGNAL_MAX - source.mDampening;
        if (newStrength < currentStrength)
            newStrength = currentStrength;
    }

    bool hasChanged = this->mStrength != newStrength;
    this->mStrength = newStrength;
    return hasChanged || this->mIsFirstTime && this->mStrength == 0;
}

void BaseCircuitComponent::addSourceItem(const CircuitComponentList::Item &info)
{
    for (auto &&source : this->mSources)
    {
        if (source.mComponent == info.mComponent && source.mDirection == info.mDirection)
        {
            source = info;
            return;
        }
    }
    this->mSources.push_back(info);
    this->mDestinationList.emplace(this, 0);
}

bool BaseCircuitComponent::trackPowerSource(const CircuitTrackingInfo &info, int dampening, bool directlyPowered, int data, bool duplicates)
{
    int newDampening = info.mDampening - 1 > 0 ? info.mDampening - 1 : 0;
    for (auto &iter : this->mSources)
    {
        if (iter.mPos != info.mPower.mPos || duplicates && iter.mDirection != info.mCurrent.mDirection)
            continue;

        if (iter.mDirectlyPowered || !directlyPowered)
        {
            if (iter.mDampening <= newDampening)
                return false;
            else
                iter.mDampening = newDampening;
            return true;
        }
        else
        {
            iter.mDirectlyPowered = true;
            if (info.mPower.mComponent && info.mPower.mComponent->getStrength() <= newDampening)
                iter.mDampening = mce::Math::max(mce::Math::min(newDampening, iter.mDampening), 0);
            else
                iter.mDampening = newDampening;
            return true;
        }
    }
    CircuitComponentList::Item value;
    value.mDampening = newDampening;
    value.mPos = info.mPower.mPos;
    value.mDirection = info.mCurrent.mDirection;
    value.mDirectlyPowered = directlyPowered;
    value.mComponent = info.mPower.mComponent;
    value.mData = data;
    this->addSourceItem(value);
    return true;
}

void BaseCircuitComponent::removeFromAnySourceList(const BaseCircuitComponent *component)
{
    for (auto iter = this->mSources.begin(); iter != this->mSources.end();)
    {
        if (iter->mComponent == component)
            iter = this->mSources.erase(iter);
        else
            ++iter;
    }
}

CircuitComponentType BaseCircuitComponent::getCircuitComponentType() const
{
    return this->mCircuitComponentType;
}

CircuitComponentType BaseCircuitComponent::getCircuitComponentGroupType() const
{
    return CircuitComponentType{(int64_t)this->getCircuitComponentType() & (int64_t)CircuitComponentType::Mask};
}

void BaseCircuitComponent::removeSource(const BlockPos &pos, const BaseCircuitComponent *component)
{
    this->mSources.removeSource(pos, component);
}

bool BaseCircuitComponent::hasSource(BaseCircuitComponent &component)
{
    if (this->mSources.size() <= 0)
        return false;
    for (auto &systemComponent : mSources.mComponents)
    {
        if (systemComponent.mComponent == &component)
            return true;

        if (systemComponent.mComponent->hasChildrenSource() && systemComponent.mComponent->hasSource(component))
            return true;
    }
    return false;
}

bool BaseCircuitComponent::hasDirectPower()
{
    for (auto &s : this->mSources)
    {
        if (s.mDirectlyPowered)
            return true;
    }
    return false;
}
