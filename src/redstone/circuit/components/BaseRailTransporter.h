#pragma once

#include "BaseCircuitComponent.h"
#include "redstone/circuit/CircuitTrackingInfo.h"

class BaseRailTransporter : public BaseCircuitComponent
{
public:
    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    virtual bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered) override;

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override;

    virtual CircuitComponentType getCircuitComponentType() const override;

    enum class RailType : int
    {
        Activator = 0,
        Power = 1,
    };

    void setType(BaseRailTransporter::RailType railType) { this->mRailType = railType; }

    RailType getType() { return this->mRailType; }

private:
    const CircuitComponentType mCircuitComponentType = CircuitComponentType::BaseRailTransporter;
    RailType                   mRailType = RailType::Activator;
};
