#pragma once

#include "BaseCircuitComponent.h"
#include "redstone/circuit/CircuitTrackingInfo.h"

class PoweredBlockComponent : public BaseCircuitComponent
{
public:
    PoweredBlockComponent(uint8_t connections = 255) { this->mIsFirstTime = false; }

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    virtual bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered) override;

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override { return false; }

    void setAllowAsPowerSource(bool bPowerSource);

    bool isAllowedAsPowerSource() { return this->mAllowAsPowerSource; }

    bool isPromotedToProducer() { return this->mPromotedToProducer; }

    virtual bool canConsumerPower() const override { return true; }

    virtual bool hasChildrenSource() override { return true; }

    virtual int getStrength() const override;

    virtual CircuitComponentType getCircuitComponentType() const override { return this->mCircuitComponentType; }

private:
    bool                       mPromotedToProducer = false;
    bool                       mAllowAsPowerSource = true;
    const CircuitComponentType mCircuitComponentType = CircuitComponentType::PoweredBlockComponent;
};