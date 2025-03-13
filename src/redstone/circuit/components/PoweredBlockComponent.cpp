#include "PoweredBlockComponent.h"

#include "ProducerComponent.h"
#include "CapacitorComponent.h"

bool PoweredBlockComponent::addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered)
{
    if (!this->mAllowAsPowerSource)
        return false;
    CircuitComponentType id = info.mNearest.mTypeID;
    bool                 oldDirectlyPowered = bDirectlyPowered;
    if (this->mAllowPowerUp)
        return false;
    this->mPromotedToProducer = false;
    if (id != CircuitComponentType::ConsumerComponent && id != CircuitComponentType::PoweredBlockComponent)
    {
        switch (id)
        {
        case CircuitComponentType::ProducerComponent: {
            ProducerComponent *producerComponent = (ProducerComponent *)info.mNearest.mComponent;
            bDirectlyPowered = producerComponent->doesAllowAttachments() && info.mCurrent.mDirection == producerComponent->getDirection();
            this->mPromotedToProducer = bDirectlyPowered;
            break;
        }
        case CircuitComponentType::CapacitorComponent:
            bDirectlyPowered = info.mCurrent.mDirection == static_cast<CapacitorComponent *>(info.mNearest.mComponent)->getPoweroutDirection();
            this->mPromotedToProducer = bDirectlyPowered;
            if (!this->mPromotedToProducer && info.mNearest.mComponent->getCircuitComponentType() == CircuitComponentType::RedstoneTorchCapacitor)
                return false;
            break;
        case CircuitComponentType::TransporterComponent:
            bDirectlyPowered = oldDirectlyPowered || ((info.mNearest.mDirection != info.mCurrent.mDirection || info.mNearest.mPos.y != info.mCurrent.mPos.y) && info.mCurrent.mDirection == 0);
            break;
        }
        return this->trackPowerSource(info, dampening, bDirectlyPowered, 0);
    }
    bDirectlyPowered = false;
    return false;
}

bool PoweredBlockComponent::allowConnection(CircuitSceneGraph &Graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    CircuitComponentType typeID = info.mCurrent.mTypeID;
    if (typeID == CircuitComponentType::TransporterComponent)
    {
        return this->mPromotedToProducer;
    }
    else
    {
        return typeID != CircuitComponentType::PoweredBlockComponent;
    }
}

void PoweredBlockComponent::setAllowAsPowerSource(bool bPowerSource)
{
    this->mAllowAsPowerSource = bPowerSource;
}

int PoweredBlockComponent::getStrength() const
{
    int newStrength = 0;
    for (auto &source : this->mSources.mComponents)
    {
        auto currentStrength = source.mComponent->getStrength() - source.mDampening;
        if (currentStrength < 0)
            currentStrength = 0;
        if (newStrength < currentStrength)
            newStrength = currentStrength;
    }
    return newStrength;
}
