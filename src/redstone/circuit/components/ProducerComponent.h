#pragma once

#include "BaseCircuitComponent.h"
#include "redstone/circuit/CircuitTrackingInfo.h"

class CircuitSceneGraph;

class ProducerComponent : public BaseCircuitComponent
{
public:
    static const uint64_t TypeID = CS_Producer;

    ProducerComponent() = default;

    virtual uint64_t getBaseType() const override;

    virtual uint64_t getInstanceType() const override;

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override;

    virtual void setStrength(int strength) override;

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    void allowAttachments(bool bAttached);

    bool doesAllowAttachments();

    virtual bool canStopPower() override;

    virtual void setStopPower(bool bPower) override;

protected:
    int mNextStrength = 0;

private:
    bool mAttachedAllowed = false;
    bool mStopPower = false;
};
