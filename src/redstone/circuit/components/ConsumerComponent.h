#pragma once

#include "BaseCircuitComponent.h"
#include "redstone/circuit/CircuitTrackingInfo.h"

class ConsumerComponent : public BaseCircuitComponent
{
public:
    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override;

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    virtual bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered) override;

    virtual bool canConsumerPower() const override { return true; }

    bool canPropagatePower() { return this->mPropagatePower; }

    void setPropagatePower(bool bPropagatePower) { this->mPropagatePower = bPropagatePower; }

    bool isPromotedToProducer() { return this->mPromotedToProducer; }

    virtual bool isSecondaryPowered() override { return this->mSecondaryPowered; }

    virtual CircuitComponentType getCircuitComponentType() const override { return this->mCircuitComponentType; }

    void setAcceptHalfPulse(bool bHalfPulse) { this->mAcceptHalfPulse = bHalfPulse; }

private:
    bool                       mSecondaryPowered = false;
    bool                       mPropagatePower = false;
    bool                       mPromotedToProducer = false;
    bool                       mAcceptHalfPulse = false;
    bool                       mAcceptSameDirection = true;
    const CircuitComponentType mCircuitComponentType = CircuitComponentType::ConsumerComponent;
};
