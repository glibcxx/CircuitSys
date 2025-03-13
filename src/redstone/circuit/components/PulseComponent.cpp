#include "PulseComponent.h"

void PulseCapacitor::setStrength(int strength)
{
    this->mNextStrength = strength;
    this->mNewPowered = strength != 0;
}

bool PulseCapacitor::allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    return info.mCurrent.mDirection == Facing::OPPOSITE_FACING[this->getDirection()];
}

bool PulseCapacitor::evaluate(CircuitSystem &system, const BlockPos &pos)
{
    ProducerComponent::evaluate(system, pos);
    this->mPowered = this->mNewPowered;
    this->mNewPowered = false;
    this->mNextStrength = 0;
    return this->mPowered;
}

FacingID PulseCapacitor::getPoweroutDirection() const
{
    return Facing::OPPOSITE_FACING[this->getDirection()];
}