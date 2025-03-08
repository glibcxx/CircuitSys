#pragma once

#include "CapacitorComponent.h"

class RepeaterCapacitor : public CapacitorComponent
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

    static const uint64_t TypeID = CS_Repeater;

    virtual uint64_t getInstanceType() const override;

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    virtual bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered) override;

    virtual bool removeSource(const BlockPos &posSource, const BaseCircuitComponent *pComponent) override;

    virtual void updateDependencies(CircuitSceneGraph &system, const BlockPos &pos) override;

    virtual bool consumePowerAnyDirection() override;

    virtual bool canConsumerPower() override;

    virtual FacingID getPoweroutDirection() const override;

    virtual void checkLock(CircuitSystem &system, const BlockPos &pos) override;

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override;

    virtual void cacheValues(CircuitSystem &system, const BlockPos &pos) override;

    virtual void setStrength(int strength) override;

    virtual int getStrength() const override;

    void setDelay(int delay);

private:
    void extendPulse();

    void delayPulse(RepeaterCapacitor::States value);

    void alternatePulse();

    static const int     NUMBER_OF_DELAYS = 4;
    States               mOnStates[5]{States::OFF_LOCKED};
    int                  mInsertAt = 0;
    bool                 mPowered = false;
    bool                 mNextPower = false;
    bool                 mLocked = true;
    int                  mPulseCount = 0;
    bool                 mPulse = false;
    bool                 mNextPulse = false;
    CircuitComponentList mSideComponents;
};
