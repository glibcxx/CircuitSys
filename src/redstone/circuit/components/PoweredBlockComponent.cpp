#include "PoweredBlockComponent.h"

#include "ProducerComponent.h"
#include "CapacitorComponent.h"

bool PoweredBlockComponent::addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening,
                                      bool &bDirectlyPowered)
{
    if (!this->mAllowAsPowerSource)
        return false;
    uint64_t id = info.mNearest.mTypeID;
    bool     oldDirectlyPowered = bDirectlyPowered;
    if (this->mAllowPowerUp)
        return false;
    this->mPromotedToProducer = false;
    if (id != CS_Consumer && id != CS_PoweredBlock)
    {
        switch (id)
        {
        case CS_Producer: {
            ProducerComponent *producerComponent = (ProducerComponent *)info.mNearest.mComponent;
            bDirectlyPowered = producerComponent->doesAllowAttachments() && info.mCurrent.mDirection == producerComponent->getDirection();
            this->mPromotedToProducer = bDirectlyPowered;
            break;
        }
        case CS_Capacitor:
            bDirectlyPowered = info.mCurrent.mDirection == static_cast<CapacitorComponent *>(info.mNearest.mComponent)->getPoweroutDirection();
            this->mPromotedToProducer = bDirectlyPowered;
            if (!this->mPromotedToProducer && info.mNearest.mComponent->getInstanceType() == CS_Torch)
                return false;
            break;
        case CS_Transporter:
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
    auto typeID = info.mCurrent.mTypeID;
    if (typeID == CS_Transporter)
    {
        return this->mPromotedToProducer;
    }
    else
    {
        return typeID != CS_PoweredBlock;
    }
}

bool PoweredBlockComponent::canConsumerPower()
{
    return true;
}

bool PoweredBlockComponent::evaluate(CircuitSystem &, const BlockPos &)
{
    return false;
}

void PoweredBlockComponent::setAllowAsPowerSource(bool bPowerSource)
{
    this->mAllowAsPowerSource = bPowerSource;
}

uint64_t PoweredBlockComponent::getBaseType() const
{
    return CS_PoweredBlock;
}

uint64_t PoweredBlockComponent::getInstanceType() const
{
    return CS_PoweredBlock;
}

PoweredBlockComponent::PoweredBlockComponent(uint8_t connections) :
    BaseCircuitComponent()
{
    this->mPromotedToProducer = false;
    this->mAllowAsPowerSource = true;
    this->mIsFirstTime = false;
}

// 充能方块的红石信号是获取的时候实时计算的,没有缓存
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

bool PoweredBlockComponent::hasChildrenSource()
{
    return true;
}

bool PoweredBlockComponent::isAllowedAsPowerSource()
{
    return this->mAllowAsPowerSource;
}

bool PoweredBlockComponent::isPromotedToProducer()
{
    return this->mPromotedToProducer;
}