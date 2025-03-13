#pragma once

#include "SidePoweredComponent.h"

class RepeaterCapacitor : public SidePoweredComponent
{
public:
    enum class States : int
    {
        OFF = 0,
        ON = 1,
        OFF_LOCKED = 2,
        ON_LOCKED = 3,
    };

    RepeaterCapacitor() {}

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    virtual bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered) override;

    virtual void updateDependencies(CircuitSceneGraph &system, const BlockPos &pos) override;

    virtual bool canConsumePowerAnyDirection() const override { return true; }

    virtual bool canConsumerPower() const override { return true; }

    virtual FacingID getPoweroutDirection() const override;

    virtual void checkLock(CircuitSystem &system, const BlockPos &pos) override;

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override;

    virtual void cacheValues(CircuitSystem &system, const BlockPos &pos) override;

    virtual void setStrength(int strength) override;

    virtual int getStrength() const override;

    virtual CircuitComponentType getCircuitComponentType() const override { return this->mCircuitComponentType; }

    void setDelay(int delay);

private:
    void extendPulse();

    void delayPulse(RepeaterCapacitor::States value);

    void alternatePulse();

    static const int           NUMBER_OF_DELAYS = 4;
    States                     mOnStates[5]{States::OFF_LOCKED};
    int                        mInsertAt = 0;
    bool                       mPowered = false;
    bool                       mNextPower = false;
    bool                       mLocked = true;
    int                        mPulseCount = 0;
    bool                       mPulse = false;
    bool                       mNextPulse = false;
    const CircuitComponentType mCircuitComponentType = CircuitComponentType::RepeaterCapacitor;
};
