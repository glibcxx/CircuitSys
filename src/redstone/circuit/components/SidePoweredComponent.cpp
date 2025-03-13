#include "SidePoweredComponent.h"

void SidePoweredComponent::removeSource(const BlockPos &posSource, const BaseCircuitComponent *pComponent)
{
    this->BaseCircuitComponent::removeSource(posSource, pComponent);
    for (auto iter = this->mSideComponents.begin(); iter != this->mSideComponents.end();)
    {
        if (iter->mPos == posSource)
            iter = this->mSideComponents.erase(iter);
        else
            ++iter;
    }
}

bool SidePoweredComponent::allowConnection(CircuitSceneGraph &, const CircuitTrackingInfo &info, bool &)
{
    return info.mCurrent.mDirection == this->getPoweroutDirection();
}

void SidePoweredComponent::removeFromAnySourceList(const BaseCircuitComponent *component)
{
    for (auto iter = this->mSources.begin(); iter != this->mSources.end();)
    {
        if (iter->mComponent == component)
            iter = this->mSources.erase(iter);
        else
            ++iter;
    }
    for (auto iter = this->mSideComponents.begin(); iter != this->mSideComponents.end();)
    {
        if (iter->mComponent == component)
            iter = this->mSideComponents.erase(iter);
        else
            ++iter;
    }
}

FacingID SidePoweredComponent::getPoweroutDirection() const
{
    return Facing::OPPOSITE_FACING[this->getDirection()];
}
