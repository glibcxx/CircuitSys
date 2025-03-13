#pragma once

#include "CapacitorComponent.h"

class PulseCapacitor : public CapacitorComponent
{
public:
    PulseCapacitor() {}

    virtual bool canConsumePowerAnyDirection() const override { return false; }

    virtual bool canConsumerPower() const override { return false; }

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    virtual FacingID getPoweroutDirection() const override;

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override;

    virtual void setStrength(int strength) override;

    virtual int getStrength() const override { return this->mStrength; }

    virtual CircuitComponentType getCircuitComponentType() const override { return this->mCircuitComponentType; }

protected:
    bool                       mPowered = false;
    bool                       mNewPowered = false;
    const CircuitComponentType mCircuitComponentType = CircuitComponentType::PulseCapacitor;
};
