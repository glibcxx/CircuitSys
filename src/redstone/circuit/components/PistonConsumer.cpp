#include "PistonConsumer.h"

// 在y轴方向上活塞方块的特殊值和激活方向相反
// 因此才来这么一个函数
void PistonConsumer::setBlockPowerFace(FacingID facing)
{
    if (facing == Facing::DOWN)
    {
        this->mBlockedFace = Facing::UP;
    }
    else
    {
        if (facing == Facing::UP)
        {
            this->mBlockedFace = Facing::DOWN;
        }
        else
        {
            this->mBlockedFace = facing;
        }
    }
}

bool PistonConsumer::consumePowerAnyDirection()
{
    return true;
}

bool PistonConsumer::addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered)
{
    if (this->mBlockedFace == info.mCurrent.mDirection)
        return false; // 从活塞推出面搜索到信号源，直接返回false
    auto mTypeId = info.mNearest.mTypeID;
    // 这里应该是电容器的背面无法激活活塞
    // 比如中继器的背面对着活塞
    if (mTypeId == CS_Capacitor && info.mCurrent.mDirection == info.mNearest.mComponent->getDirection())
    {
        return false;
    }

    // 对于普通充能方块，bDirectlyPowered 表示充能方块是否被充能
    // 比如拉杆可以搜索到非附着面的充能方块，充能方块也会添加拉杆为信号源
    // 但此时 bDirectlyPowered 为 false
    if (mTypeId == CS_PoweredBlock && !bDirectlyPowered)
    {
        return false;
    }
    bool bBackward = info.mCurrent.mDirection == Facing::OPPOSITE_FACING[this->getDirection()];
    this->trackPowerSource(info, dampening, bDirectlyPowered, bBackward);
    return false;
}

bool PistonConsumer::allowConnection(CircuitSceneGraph &Graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    return true;
}

uint64_t PistonConsumer::getInstanceType() const
{
    return CS_Piston;
}