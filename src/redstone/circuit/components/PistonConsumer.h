#pragma once

#include "ConsumerComponent.h"

class PistonConsumer : public ConsumerComponent
{
public:
    static const uint64_t TypeID = CS_Piston;

    virtual uint64_t getInstanceType() const override;

    PistonConsumer() = default;

    virtual ~PistonConsumer() = default;

    virtual bool consumePowerAnyDirection() override;

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    void setBlockPowerFace(FacingID blockFace);

    virtual bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered) override;

private:
    FacingID mBlockedFace = Facing::NOT_DEFINED;
};
