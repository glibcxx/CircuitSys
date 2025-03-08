#pragma once

#include "CapacitorComponent.h"

class ComparatorCapacitor : public CapacitorComponent
{
public:
    ComparatorCapacitor() = default;

    static const uint64_t TypeID = CS_Comparator;

    uint64_t getInstanceType() const;

    bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered);

    bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered);

    bool consumePowerAnyDirection();

    bool canConsumerPower();

    bool evaluate(CircuitSystem &system, const BlockPos &pos);

    int GetRearStrength();

    int GetSideStrength();

    void cacheValues(CircuitSystem &system, const BlockPos &pos);

    enum class Mode : int
    {
        COMPARE_MODE = 0,
        SUBTRACT_MODE = 1,
    };

    bool isSubtractMode();

    void setMode(ComparatorCapacitor::Mode mode);

    void setAnalogStrength(int strength, FacingID dir);

    void clearAnalogStrength(FacingID dir);

    FacingID getPoweroutDirection() const;

    int getOldStrength();

    void updateDependencies(CircuitSceneGraph &system, const BlockPos &pos);

    bool removeSource(const BlockPos &posSource, const BaseCircuitComponent *pComponent);

    void setCachedStrength(int iStrength);

private:
    int                  mRearAnalogStrength = -1;
    int                  mSideAnalogStrengthRight = -1;
    int                  mSideAnalogStrengthLeft = -1;
    int                  mOldStrength = 0;
    Mode                 mMode = Mode::COMPARE_MODE;
    int                  mRearStrength = 0;
    int                  mSideStrengths = 0;
    bool                 mHasAnalogBeenSet = false;
    CircuitComponentList mSideComponents;
};
