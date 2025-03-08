#include "BaseRailTransporter.h"
#include "Facing.h"

/**
 * @brief 将info内的信号源信息添加到trasnporter的信号源列表中
 * @param trasnporter 铁轨对象
 * @param info 被打包的信号源信息
 * @param dampening 阻尼，实际插入信号源列表的等于`dampening - 1`
 * @param distance 距离？最后会插入`distance - 1`到信号源的`mData`中
 * @return 是否添加成功
 */
bool insertIntoEntries(BaseRailTransporter *trasnporter, const CircuitTrackingInfo &info, int dampening, int distance)
{                                              
    for (auto &source : trasnporter->mSources) // 信号源列表已经有了就不用再添加，从而导致这个bug: https://bugs.mojang.com/browse/MCPE-81987，
                                               // 该bug在1.16.100.51 beta修复，然后就出现了著名的铁轨bud: https://bugs.mojang.com/browse/MCPE-105749
    {
        if (source.mPos == info.mPower.mPos)
        {
            return false;
        }
    }
    CircuitComponentList::Item value;
    value.mDampening = dampening - 1 >= 0 ? dampening - 1 : 0;
    value.mPos = info.mPower.mPos;
    value.mDirection = info.mCurrent.mDirection;
    value.mDirectlyPowered = true;
    value.mComponent = info.mPower.mComponent;
    value.mData = distance - 1 >= 0 ? distance - 1 : 0;
    trasnporter->mSources.push_back(value);
    return true;
}

uint64_t BaseRailTransporter::getBaseType() const
{
    return CS_BaseRail;
}

uint64_t BaseRailTransporter::getInstanceType() const
{
    return CS_BaseRail;
}

bool BaseRailTransporter::addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered)
{
    if (!bDirectlyPowered)
    {
        return false;
    }
    BaseCircuitComponent *current = info.mNearest.mComponent; // 上一个被搜索到的元件
    uint64_t              id = current->getInstanceType();
    bool                  bPrevDirectlyPowered = bDirectlyPowered;
    if (id == CS_BaseRail) // 该元件是铁轨
    {
        CircuitTrackingInfo newInfo(info);
        int                 distance = 0;
        int                 nSources = this->mSources.size();
        for (auto &entry : current->mSources) // 遍历该铁轨元件的父元件
        {
            if (entry.mData - 1 > 0)
            {
                // 将newInfo搜索表的信号源替换为该铁轨元件的父元件
                newInfo.mPower.mPos = entry.mPos;
                newInfo.mPower.mDirection = this->mDirection;
                newInfo.mDampening = entry.mDampening;
                newInfo.mData = entry.mData;
                insertIntoEntries(this, newInfo, newInfo.mDampening + 1, newInfo.mData);
                if (newInfo.mData > distance)
                    distance = newInfo.mData;
            }
        }
        return this->mSources.size() != nSources;
    }
    else // 如果不是铁轨
    {
        return (id != CS_PoweredBlock || bPrevDirectlyPowered) && insertIntoEntries(this, info, dampening, 10); // 直接开插
    }
}

bool BaseRailTransporter::allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    if (info.mCurrent.mComponent->getInstanceType() == CS_BaseRail && this->mRailType == ((BaseRailTransporter *)info.mCurrent.mComponent)->mRailType)
    {
        if (info.mCurrent.mComponent->getDirection() == this->mDirection)
        {
            if (info.mCurrent.mDirection != this->mDirection)
                return Facing::OPPOSITE_FACING[this->mDirection] == info.mCurrent.mDirection;
            return true;
        }
        return false;
    }
    else
    {
        return false;
    }
}

bool BaseRailTransporter::evaluate(CircuitSystem &system, const BlockPos &pos)
{
    int bPrevStrength = this->mStrength;
    this->mStrength = 0;
    for (auto &source : this->mSources)
    {
        int distance = source.mData;
        if (distance > this->mStrength && source.mComponent && (source.mComponent->getStrength() - source.mDampening) > 0)
        {
            this->mStrength = distance;
        }
    }
    return this->mStrength != bPrevStrength;
}

void BaseRailTransporter::setType(RailType railType)
{
    this->mRailType = railType;
}

BaseRailTransporter::RailType BaseRailTransporter::getType()
{
    return this->mRailType;
}
