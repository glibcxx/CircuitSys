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

    virtual FacingID getPoweroutDirection() const override { return Facing::UP; }

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered) override;

    virtual bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered) override;

    virtual void removeSource(const BlockPos &posSource, const BaseCircuitComponent *pComponent) override;

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override;

    virtual void cacheValues(CircuitSystem &system, const BlockPos &pos) override;

    virtual int getStrength() const override;

    virtual CircuitComponentType getCircuitComponentType() const override { return this->mCircuitComponentType; }

    void setOn(bool bOn);

    static constexpr int BURN_OUT_COUNT = 16;
    static constexpr int BURN_OUT_REST_COUNT = 32;
    static constexpr int LOOP_CYCLE = 5;

    void setNextInQueue(RedstoneTorchCapacitor *torch);

    int getSelfPowerCount();

    void setSelfPowerCount(int count);

    virtual bool isHalfPulse() override;

    void resetBurnOutCount();

private:
    class State
    {
    public:
        bool mOn = false;
        bool mHalfFrame = false;
        bool mChanged;
    };

    bool _canIncrementSelfPower();

    int _findStrongestStrength(const BlockPos &targetPos, CircuitSystem &system, bool &bPowerFromSelf);

    RedstoneTorchCapacitor    *mNextOrder = nullptr;
    int                        mSelfPowerCount = 0;
    State                      mState[2]{};
    bool                       mCanReigniteFromBurnout = false;
    const CircuitComponentType mCircuitComponentType = CircuitComponentType::RedstoneTorchCapacitor;

    virtual void updateDependencies(CircuitSceneGraph &system, const BlockPos &pos) override;
};