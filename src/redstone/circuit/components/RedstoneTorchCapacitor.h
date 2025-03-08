#pragma once

#include <iostream>
#include <unordered_map>
#include <queue>
#include "CapacitorComponent.h"

class RedstoneTorchCapacitor : public CapacitorComponent
{
public:
    RedstoneTorchCapacitor()
    {
        this->BaseCircuitComponent::setAllowPowerUp(true);
    }

    static const uint64_t TypeID = CS_Torch;

    virtual uint64_t getInstanceType() const override;

    virtual FacingID getPoweroutDirection() const override;

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    virtual bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered) override;

    virtual bool removeSource(const BlockPos &posSource, const BaseCircuitComponent *pComponent) override;

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override;

    virtual void cacheValues(CircuitSystem &system, const BlockPos &pos) override;

    virtual int getStrength() const override;

    void setOn(bool bOn);

    static const int BURN_OUT_COUNT = 16;
    static const int BURN_OUT_REST_COUNT = 32;
    static const int LOOP_CYCLE = 5;

    void setNextInQueue(RedstoneTorchCapacitor *torch);

    int getSelfPowerCount();

    void setSelfPowerCount(int count);

    virtual bool isHalfPulse() override;

    void resetBurnOutCount();

private:
    class State
    {
    public:
        bool mOn;
        bool mHalfFrame;
        bool mChanged;
    };

    bool _canIncrementSelfPower();

    int FindStrongestStrength(const BlockPos &targetPos, CircuitSystem &system, bool &bPowerFromSelf);

    RedstoneTorchCapacitor *mNextOrder = nullptr;
    int                     mSelfPowerCount = 0;
    State                   mState[2] = {
        {0, 0},
        {0, 0}
    };
    bool mCanReigniteFromBurnout = false;

    virtual void updateDependencies(CircuitSceneGraph &system, const BlockPos &pos) override;
};