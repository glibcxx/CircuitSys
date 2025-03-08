#pragma once

#include "BaseCircuitComponent.h"
#include "redstone/circuit/CircuitTrackingInfo.h"

class ConsumerComponent : public BaseCircuitComponent
{
public:
    static const uint64_t TypeID = CS_Consumer;

    virtual uint64_t getBaseType() const override;

    virtual uint64_t getInstanceType() const override;

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override;

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    virtual bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered) override;

    virtual bool canConsumerPower() override;

    bool canPropagatePower();

    void setPropagatePower(bool bPropagatePower);

    bool isPromotedToProducer();

    virtual bool isSecondaryPowered() override;

    void setAcceptHalfPulse(bool bHalfPulse);

private:
    bool mSecondaryPowered = false;
    bool mPropagatePower = false;
    bool mPromotedToProducer = false;
    bool mAcceptHalfPulse = false;
};
