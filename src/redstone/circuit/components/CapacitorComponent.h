#pragma once

#include "ProducerComponent.h"
#include "redstone/circuit/CircuitTrackingInfo.h"

class CapacitorComponent : public ProducerComponent
{
public:
    const CircuitComponentType mCircuitComponentType = CircuitComponentType::CapacitorComponent;

    virtual CircuitComponentType getCircuitComponentType() const override;

    virtual FacingID getPoweroutDirection() const;
};
