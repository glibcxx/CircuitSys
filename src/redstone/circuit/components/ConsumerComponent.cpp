#include "ConsumerComponent.h"

#include "ProducerComponent.h"
#include "CapacitorComponent.h"

bool ConsumerComponent::addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered)
{
    CircuitComponentType id = info.mNearest.mTypeID;
    bool                 bPrevDirectlyPowered = bDirectlyPowered;
    bDirectlyPowered = false;
    this->mPromotedToProducer = false;
    if (this->mPropagatePower)
    {
        switch (id)
        {
        case CircuitComponentType::ProducerComponent: {
            ProducerComponent *producer = static_cast<ProducerComponent *>(info.mNearest.mComponent);
            bDirectlyPowered = producer->doesAllowAttachments() && info.mCurrent.mDirection == producer->getDirection();
            this->mPromotedToProducer = bDirectlyPowered;
            break;
        }
        case CircuitComponentType::CapacitorComponent: {
            bDirectlyPowered = info.mCurrent.mDirection == Facing::UP;
            FacingID dirPower = info.mNearest.mComponent->getDirection();

            if (dirPower != Facing::NOT_DEFINED)
            {
                CapacitorComponent *capacitor = static_cast<CapacitorComponent *>(info.mNearest.mComponent);
                if (capacitor->getPoweroutDirection() != Facing::NOT_DEFINED)
                    dirPower = capacitor->getPoweroutDirection();
                bDirectlyPowered = info.mCurrent.mDirection == dirPower;
                this->mPromotedToProducer = bDirectlyPowered;
            }
            if (!this->mAcceptSameDirection && info.mCurrent.mDirection == info.mNearest.mComponent->getDirection())
                return false;
            break;
        }
        case CircuitComponentType::TransporterComponent:
            bDirectlyPowered = true;
            break;
        default:
            if (id == CircuitComponentType::PoweredBlockComponent && !bPrevDirectlyPowered)
                return false;
            break;
        }
    }
    else if (id == CircuitComponentType::PoweredBlockComponent && !bPrevDirectlyPowered)
    {
        return false;
    }
    return this->trackPowerSource(info, dampening, bDirectlyPowered, id == CircuitComponentType::ConsumerComponent) && bDirectlyPowered;
}

bool ConsumerComponent::allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    CircuitComponentType id = info.mCurrent.mTypeID;

    if (id == CircuitComponentType::CapacitorComponent)
    {
        FacingID dirPower = static_cast<CapacitorComponent *>(info.mCurrent.mComponent)->getPoweroutDirection();

        if (dirPower == Facing::NOT_DEFINED)
        {
            return info.mCurrent.mDirection == 1 && info.mDirectlyPowered;
        }
        else
        {
            return info.mCurrent.mDirection == dirPower && info.mDirectlyPowered;
        }
    }
    else if (id == CircuitComponentType::TransporterComponent && this->mPropagatePower)
    {
        return this->mPromotedToProducer;
    }
    else
    {
        return info.mDirectlyPowered;
    }
}

bool ConsumerComponent::evaluate(CircuitSystem &, const BlockPos &)
{
    int newStrength = 0;
    this->mSecondaryPowered = 0;
    for (auto &source : this->mSources.mComponents)
    {
        int currentStrength = source.mComponent->getStrength() - source.mDampening;
        if (source.mComponent->isHalfPulse() && !this->mAcceptHalfPulse)
        {
            currentStrength = Redstone::SIGNAL_MAX - source.mDampening;
        }
        if (currentStrength < 0)
            currentStrength = 0;
        if (newStrength < currentStrength)
        {
            this->mSecondaryPowered = source.mData == 1;
            newStrength = currentStrength;
        }
    }
    bool hasChanged = this->mStrength != newStrength;
    this->mStrength = newStrength;
    return hasChanged || this->mIsFirstTime;
}
