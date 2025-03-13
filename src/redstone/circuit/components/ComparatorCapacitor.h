#pragma once

#include "SidePoweredComponent.h"

class ComparatorCapacitor : public SidePoweredComponent
{
public:
    ComparatorCapacitor() {}

    bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered);

    virtual bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered) override;

    bool consumePowerAnyDirection() { return true; }

    bool canConsumerPower() { return true; }

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) override;

    int GetRearStrength();

    int GetSideStrength();

    virtual void cacheValues(CircuitSystem &system, const BlockPos &pos) override;

    enum class Mode : int
    {
        COMPARE_MODE = 0,
        SUBTRACT_MODE = 1,
    };

    bool isSubtractMode() { return this->mMode == Mode::SUBTRACT_MODE; }

    void setMode(ComparatorCapacitor::Mode mode) { this->mMode = mode; }

    void setAnalogStrength(int strength, FacingID dir);

    void clearAnalogStrength(FacingID dir);

    virtual FacingID getPoweroutDirection() const override;

    int getOldStrength() { return this->mOldStrength; }

    virtual void updateDependencies(CircuitSceneGraph &system, const BlockPos &pos) override;

    virtual CircuitComponentType getCircuitComponentType() const override;

    void setCachedStrength(int iStrength);

private:
    int                        mRearAnalogStrength = -1;
    int                        mSideAnalogStrengthRight = -1;
    int                        mSideAnalogStrengthLeft = -1;
    int                        mOldStrength = 0;
    Mode                       mMode = Mode::COMPARE_MODE;
    int                        mRearStrength = 0;
    int                        mSideStrengths = 0;
    bool                       mHasAnalogBeenSet = false;
    const CircuitComponentType mCircuitComponentType = CircuitComponentType::ComparatorCapacitor;
};
