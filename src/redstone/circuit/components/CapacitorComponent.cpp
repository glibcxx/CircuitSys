#include "CapacitorComponent.h"

uint64_t CapacitorComponent::getBaseType() const
{
    return CS_Capacitor;
}

uint64_t CapacitorComponent::getInstanceType() const
{
    return CS_Capacitor;
}

FacingID CapacitorComponent::getPoweroutDirection() const
{
    return Facing::NOT_DEFINED;
}