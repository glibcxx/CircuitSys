#pragma once

#include <map>
#include "CircuitComponentList.h"
#include "redstone/Redstone.h"

class CircuitSystem;
class CircuitSceneGraph;
class CircuitTrackingInfo;

class BaseCircuitComponent
{
public:
    BaseCircuitComponent() {}

    virtual ~BaseCircuitComponent() {}

    virtual int getStrength() const { return this->mStrength; }

    virtual FacingID getDirection() const { return this->mDirection; }

    virtual void setStrength(int strength) { this->mStrength = strength; }

    virtual void setDirection(FacingID direction) { this->mDirection = direction; }

    bool hasDirectPower();

    virtual void setConsumePowerAnyDirection(bool consume) { this->mConsumePowerAnyDirection = consume; }

    virtual bool canConsumePowerAnyDirection() const { return this->mConsumePowerAnyDirection; }

    virtual bool canConsumerPower() const { return false; }

    bool needUpdating() { return this->mNeedsUpdate; }

    void clearUpdatingFlag() { this->mNeedsUpdate = false; }

    virtual bool canStopPower() const { return false; }

    virtual void setStopPower(bool bPower) {}

    CircuitComponentList mSources{};

    virtual void removeSource(const BlockPos &posSource, const BaseCircuitComponent *pComponent);

    virtual bool addSource(CircuitSceneGraph         &graph,
                           const CircuitTrackingInfo &info,
                           int                       &dampening,
                           bool                      &bDirectlyPowered) { return false; }

    virtual bool allowConnection(CircuitSceneGraph         &graph,
                                 const CircuitTrackingInfo &info,
                                 bool                      &bDirectlyPowered) { return true; }

    virtual void checkLock(CircuitSystem &system, const BlockPos &pos) {}

    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) { return false; }

    virtual void cacheValues(CircuitSystem &system, const BlockPos &pos) {}

    virtual void updateDependencies(CircuitSceneGraph &system, const BlockPos &pos) {}

    virtual bool allowIndirect() { return false; }

    void setAllowPowerUp(bool bOnlyPowerUp) { this->mAllowPowerUp = bOnlyPowerUp; }

    bool canAllowPowerUp() { return this->mAllowPowerUp; }

    bool canAllowPowerDown() { return this->mAllowPowerDown; }

    void setAllowPowerDown(bool bOnlyPowerDown) { this->mAllowPowerDown = bOnlyPowerDown; }

    virtual bool isHalfPulse() { return false; }

    virtual bool hasSource(BaseCircuitComponent &source);

    virtual bool hasChildrenSource() { return false; }

    void clearFirstTimeFlag() { this->mIsFirstTime = false; }

    virtual bool isSecondaryPowered() { return false; }

    virtual void removeFromAnySourceList(const BaseCircuitComponent *component);

    virtual CircuitComponentType getCircuitComponentType() const;

    virtual CircuitComponentType getCircuitComponentGroupType() const;

    void setRemoved() { this->mRemoved = true; }

    bool isRemoved() const { return this->mRemoved; }

    void addSourceItem(const CircuitComponentList::Item &info);

    std::map<BaseCircuitComponent *, int> mDestinationList;
    bool                                  mIgnoreFirstUpdate = false;
    bool                                  mIsFirstTime = true;
    bool                                  mNeedsUpdate = false;
    BlockPos                              mPos{};
    BlockPos                              mChunkPosition{};
    bool                                  mShouldEvaluate = true;

protected:
    int      mStrength = 0;
    FacingID mDirection = Facing::NOT_DEFINED;

    bool trackPowerSource(const CircuitTrackingInfo &info, int dampening, bool directlyPowered, int data, bool duplicates = false);

    bool calculateValue(CircuitSystem &system);

    bool mAllowPowerUp = false;
    bool mAllowPowerDown = true;

private:
    bool                 mRemoved = false;
    bool                 mConsumePowerAnyDirection;
    CircuitComponentType mCircuitComponentType;
};
