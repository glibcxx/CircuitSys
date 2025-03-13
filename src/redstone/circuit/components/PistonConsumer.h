#pragma once

#include "ConsumerComponent.h"

class PistonConsumer : public ConsumerComponent
{
public:
    PistonConsumer() {}

    virtual ~PistonConsumer() {}

    virtual bool canConsumePowerAnyDirection() const override { return true; }

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override { return true; }

    void setBlockPowerFace(FacingID blockFace);

    virtual bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered) override;

    virtual CircuitComponentType getCircuitComponentType() const override { return this->mCircuitComponentType; }

private:
    FacingID                   mBlockedFace = Facing::NOT_DEFINED;
    const CircuitComponentType mCircuitComponentType = CircuitComponentType::PistonConsumer;
};
