#include "ProducerComponent.h"

bool ProducerComponent::allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    if (this->mAttachedAllowed)
        return true;
    return this->getCircuitComponentGroupType() != CircuitComponentType::PoweredBlockComponent;
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
