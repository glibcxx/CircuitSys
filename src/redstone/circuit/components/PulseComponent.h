#pragma once

#include "CapacitorComponent.h"

class PulseCapacitor : public CapacitorComponent
{
public:
    static const uint64_t TypeID = CS_Pulse;

public:
    PulseCapacitor() = default;

    virtual uint64_t getInstanceType() const override;

    virtual bool consumePowerAnyDirection() override;

    virtual bool canConsumerPower() override;

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    virtual FacingID getPoweroutDirection() const override;

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override;

    virtual void setStrength(int strength) override;

    virtual int getStrength() const override;

protected:
    bool mPowered = false;
    bool mNewPowered = false;
};
