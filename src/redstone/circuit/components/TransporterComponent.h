#pragma once

#include "BaseCircuitComponent.h"
#include "redstone/circuit/CircuitTrackingInfo.h"

class TransporterComponent : public BaseCircuitComponent
{
public:
    static const uint64_t TypeID = CS_Transporter;

    virtual uint64_t getBaseType() const override;

    virtual uint64_t getInstanceType() const override;

    TransporterComponent() = default;

    virtual void cacheValues(CircuitSystem &system, const BlockPos &pos) override;

    virtual bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered) override;

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    virtual bool canConsumerPower() override;

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override;

    int mNextStrength = 0;
};