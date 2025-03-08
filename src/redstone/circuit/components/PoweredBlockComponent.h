#pragma once

#include "BaseCircuitComponent.h"
#include "redstone/circuit/CircuitTrackingInfo.h"

class PoweredBlockComponent : public BaseCircuitComponent
{
public:
    static const uint64_t TypeID = CS_PoweredBlock;

    virtual uint64_t getBaseType() const override;

    virtual uint64_t getInstanceType() const override;

    PoweredBlockComponent(uint8_t connections);

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    virtual bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered) override;

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override;

    void setAllowAsPowerSource(bool bPowerSource);

    bool isAllowedAsPowerSource();

    bool isPromotedToProducer();

    virtual bool canConsumerPower() override;

    virtual bool hasChildrenSource() override;

    virtual int getStrength() const override;

private:
    bool mPromotedToProducer = false;
    bool mAllowAsPowerSource = true;
};