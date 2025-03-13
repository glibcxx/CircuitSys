#pragma once

#include "BaseCircuitComponent.h"
#include "redstone/circuit/CircuitTrackingInfo.h"

class TransporterComponent : public BaseCircuitComponent
{
public:
    TransporterComponent() {}

    virtual void cacheValues(CircuitSystem &system, const BlockPos &pos) override;

    virtual bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered) override;

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    virtual bool canConsumerPower() const override;

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override;

    virtual CircuitComponentType getCircuitComponentType() const override { return this->mCircuitComponentType; }

    int                        mNextStrength = 0;
    const CircuitComponentType mCircuitComponentType = CircuitComponentType::TransporterComponent;
};