#pragma once

#include "BaseCircuitComponent.h"
#include "redstone/circuit/CircuitTrackingInfo.h"

class BaseRailTransporter : public BaseCircuitComponent
{
public:
    static const uint64_t TypeID = CS_BaseRail;

    virtual uint64_t getBaseType() const override;

    virtual uint64_t getInstanceType() const override;

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    virtual bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered) override;

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override;

    enum class RailType : int
    {
        Activator = 0,
        Power = 1,
    };

    void setType(BaseRailTransporter::RailType railType);

    RailType getType();

private:
    RailType mRailType = RailType::Activator;
};
