#pragma once

#include "CircuitComponentList.h"
#include "redstone/Redstone.h"

class CircuitSystem;
class CircuitSceneGraph;
class CircuitTrackingInfo;

class BaseCircuitComponent
{
public:
    BaseCircuitComponent() = default;

    virtual ~BaseCircuitComponent() = default;

    virtual int getStrength() const;

    virtual FacingID getDirection() const;

    virtual void setStrength(int strength);

    virtual void setDirection(FacingID direction);

    bool hasDirectPower();

    virtual bool consumePowerAnyDirection();

    virtual bool canConsumerPower();

    bool needUpdating();

    void clearUpdatingFlag();

    virtual bool canStopPower();

    virtual void setStopPower(bool bPower);

    static const uint64_t TypeID = CS_BaseCircuit;

    CircuitComponentList mSources;

    virtual uint64_t getBaseType() const;

    virtual uint64_t getInstanceType() const;

    virtual bool removeSource(const BlockPos &posSource, const BaseCircuitComponent *pComponent);

    virtual bool addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered);

    virtual bool allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered);

    virtual void checkLock(CircuitSystem &system, const BlockPos &pos);

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos);

    virtual void cacheValues(CircuitSystem &system, const BlockPos &pos);

    virtual void updateDependencies(CircuitSceneGraph &system, const BlockPos &pos);

    virtual bool allowIndirect();

    void setAllowPowerUp(bool bOnlyPowerDown);

    bool canAllowPowerUp();

    bool canAllowPowerDown();

    void setAllowPowerDown(bool bOnlyPowerDown);

    virtual bool isHalfPulse();

    virtual bool hasSource(BaseCircuitComponent &source);

    virtual bool hasChildrenSource();

    void clearFirstTimeFlag();

    virtual bool isSecondaryPowered();

    void setRemoved();

    bool isRemoved() const;

    bool     mIgnoreFirstUpdate = false;
    bool     mIsFirstTime = true;
    bool     mNeedsUpdate = false;
    BlockPos mChunkPosition;
    bool     mShouldEvaluate = true;

protected:
    int      mStrength = 0;
    FacingID mDirection = Facing::NOT_DEFINED;

    bool trackPowerSource(const CircuitTrackingInfo &info, int dampening, bool directlyPowered, int data);

    bool trackPowerSourceDuplicates(const CircuitTrackingInfo &info, int dampening, bool directlyPowered);

    bool calculateValue(CircuitSystem &system);

    bool mAllowPowerUp = false;
    bool mAllowPowerDown = true;

private:
    bool mRemoved = false;
};
