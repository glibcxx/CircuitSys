#pragma once

#include "ProducerComponent.h"
#include "redstone/circuit/CircuitTrackingInfo.h"

class CapacitorComponent : public ProducerComponent
{
public:
    static const uint64_t TypeID = CS_Capacitor;

    virtual uint64_t getBaseType() const override;

    virtual uint64_t getInstanceType() const override;

    virtual FacingID getPoweroutDirection() const;
};
