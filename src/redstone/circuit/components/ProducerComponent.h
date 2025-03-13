#pragma once

#include "BaseCircuitComponent.h"
#include "redstone/circuit/CircuitTrackingInfo.h"

class CircuitSceneGraph;

class ProducerComponent : public BaseCircuitComponent
{
public:
    ProducerComponent() {}

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override;

    virtual void setStrength(int strength) override;

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    void allowAttachments(bool bAttached) { this->mAttachedAllowed = bAttached; }

    bool doesAllowAttachments() { return this->mAttachedAllowed; }

    virtual bool canStopPower() const override { return this->mStopPower; }

    virtual void setStopPower(bool bPower) override { this->mStopPower = bPower; }

    virtual CircuitComponentType getCircuitComponentType() const override { return this->mCircuitComponentType; }

protected:
    int mNextStrength = 0;

private:
    bool                       mAttachedAllowed = false;
    bool                       mStopPower = false;
    const CircuitComponentType mCircuitComponentType = CircuitComponentType::ProducerComponent;
};
