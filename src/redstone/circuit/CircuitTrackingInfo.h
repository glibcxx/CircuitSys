#pragma once

#include "level/BlockPos.h"
#include "Facing.h"
#include "components/BaseCircuitComponent.h"

/**
 * @brief 元件搜索信息表，信号源寻找子元件时，用这个保存搜索信息
 *
 */
class CircuitTrackingInfo
{
public:
    struct Entry
    {
        BaseCircuitComponent *mComponent = nullptr;
        BlockPos              mPos;
        FacingID              mDirection = Facing::DOWN;
        uint64_t              mTypeID = CS_BaseCircuit;

        void set(BaseCircuitComponent *component, const BlockPos &pos, const FacingID &direction, const uint64_t typeID)
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

    CircuitTrackingInfo(BaseCircuitComponent *component, const BlockPos &pos, int dampening)
    {
        this->mCurrent.mPos = pos;
        this->mCurrent.mComponent = component;
        this->mCurrent.mTypeID = component->getBaseType();
        this->mCurrent.mDirection = component->getDirection();
        this->mPower = this->m2ndNearest = this->mNearest = this->mCurrent;
        this->mDampening = dampening;
        this->mDirectlyPowered = true;
        this->mData = 0;
    }
};
