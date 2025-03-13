#pragma once

#include <gsl/gsl>
#include "level/BlockPos.h"
#include "Facing.h"
#include "components/BaseCircuitComponent.h"

class CircuitTrackingInfo
{
public:
    struct Entry
    {
        BaseCircuitComponent *mComponent = nullptr;
        BlockPos              mPos;
        FacingID              mDirection = Facing::DOWN;
        CircuitComponentType  mTypeID = CircuitComponentType::Undefined;

        void set(BaseCircuitComponent *component, const BlockPos &pos, const FacingID &direction, const CircuitComponentType typeID)
        {
            this->mComponent = component;
            this->mTypeID = typeID;
            this->mDirection = direction;
            this->mPos = pos;
        }
    };

    Entry mCurrent;
    Entry mPower;
    Entry mNearest;
    Entry m2ndNearest;
    int   mDampening;
    bool  mDirectlyPowered = true;
    int   mData = 0;

    CircuitTrackingInfo(gsl::not_null<BaseCircuitComponent *> component, const BlockPos &pos, int dampening)
    {
        this->mCurrent.mPos = pos;
        this->mCurrent.mComponent = component;
        this->mCurrent.mTypeID = component->getCircuitComponentGroupType();
        this->mCurrent.mDirection = component->getDirection();
        this->mPower = this->m2ndNearest = this->mNearest = this->mCurrent;
        this->mDampening = dampening;
        this->mDirectlyPowered = true;
        this->mData = 0;
    }
};
