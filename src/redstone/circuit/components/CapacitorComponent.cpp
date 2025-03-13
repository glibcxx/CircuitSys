#include "CapacitorComponent.h"

CircuitComponentType CapacitorComponent::getCircuitComponentType() const
{
    return this->mCircuitComponentType;
}

FacingID CapacitorComponent::getPoweroutDirection() const
{
    return Facing::NOT_DEFINED;
}
