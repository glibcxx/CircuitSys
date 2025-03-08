#include "ProducerComponent.h"

void ProducerComponent::allowAttachments(bool attached)
{
    this->mAttachedAllowed = attached;
}

bool ProducerComponent::allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    if (this->mAttachedAllowed)
        return true;
    return this->getBaseType() != CS_PoweredBlock;
}

bool ProducerComponent::canStopPower()
{
    return this->mStopPower;
}

void ProducerComponent::setStopPower(bool bPower)
{
    this->mStopPower = bPower;
}

bool ProducerComponent::doesAllowAttachments()
{
    return this->mAttachedAllowed;
}

uint64_t ProducerComponent::getBaseType() const
{
    return CS_Producer;
}

uint64_t ProducerComponent::getInstanceType() const
{
    return CS_Producer;
}

void ProducerComponent::setStrength(int strength)
{
    this->mNextStrength = strength;
    if (this->mIsFirstTime)
        this->mStrength = mNextStrength;
}

bool ProducerComponent::evaluate(CircuitSystem &, const BlockPos &)
{
    this->mStrength = this->mNextStrength;
    return false;
}
