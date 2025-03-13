#pragma once

#include "ProducerComponent.h"
#include "redstone/circuit/CircuitTrackingInfo.h"

class CapacitorComponent : public ProducerComponent
{
    const CircuitComponentType mCircuitComponentType = CircuitComponentType::CapacitorComponent;

public:
    virtual CircuitComponentType getCircuitComponentType() const override;

    virtual FacingID getPoweroutDirection() const;
};
