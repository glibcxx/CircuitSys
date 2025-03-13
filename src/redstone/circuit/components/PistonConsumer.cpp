#include "PistonConsumer.h"

void PistonConsumer::setBlockPowerFace(FacingID facing)
{
    if (facing == Facing::DOWN)
        this->mBlockedFace = Facing::UP;
    else if (facing == Facing::UP)
        this->mBlockedFace = Facing::DOWN;
    else
        this->mBlockedFace = facing;
}

bool PistonConsumer::addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered)
{
    if (this->mBlockedFace == info.mCurrent.mDirection)
        return false;
    CircuitComponentType mTypeId = info.mNearest.mTypeID;
    if (mTypeId == CircuitComponentType::CapacitorComponent && info.mCurrent.mDirection == info.mNearest.mComponent->getDirection())
        return false;

    if (mTypeId == CircuitComponentType::PoweredBlockComponent && !bDirectlyPowered)
        return false;

    bool bBackward = info.mCurrent.mDirection == Facing::OPPOSITE_FACING[this->getDirection()];
    this->trackPowerSource(info, dampening, bDirectlyPowered, bBackward);
    return false;
}
