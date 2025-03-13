#pragma once

#include "CapacitorComponent.h"

class SidePoweredComponent : public ::CapacitorComponent
{
protected:
    CircuitComponentList mSideComponents{};

public:
    virtual ~SidePoweredComponent()
    {
        for (auto &&iter : this->mSideComponents)
        {
            iter.mComponent->mDestinationList.erase(this);
        }
    }

    virtual bool canConsumePowerAnyDirection() const override { return true; }

    virtual bool canConsumerPower() const override { return true; }

    virtual void removeSource(const BlockPos &posSource, const BaseCircuitComponent *pComponent) override;

    virtual bool allowConnection(CircuitSceneGraph &, const CircuitTrackingInfo &info, bool &) override;

    virtual void removeFromAnySourceList(const BaseCircuitComponent *component) override;

    virtual FacingID getPoweroutDirection() const;
};
