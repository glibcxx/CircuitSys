#include "ConsumerComponent.h"

#include "ProducerComponent.h"
#include "CapacitorComponent.h"

bool ConsumerComponent::addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered)
{
    /*
        对于消费者，this->mPromotedToProducer 表示自身被强充能，
        bDirectlyPowered 表示自己被充能
        对于普通的充能方块，bDirectlyPowered 表示自己是否被充能
        形参中的bDirectlyPowered的值是info.mNearest的值
    */
    uint64_t id = info.mNearest.mTypeID;
    bool     bPrevDirectlyPowered = bDirectlyPowered;
    bDirectlyPowered = false;
    this->mPromotedToProducer = false;
    if (this->mPropagatePower)
    {
        switch (id)
        {
        case CS_Producer: { // 从生产者搜索过来
            ProducerComponent *producer = static_cast<ProducerComponent *>(info.mNearest.mComponent);
            // 能强充能就能升为生产者，比如拉杆附着在方块上
            bDirectlyPowered = producer->doesAllowAttachments() && info.mCurrent.mDirection == producer->getDirection();
            this->mPromotedToProducer = bDirectlyPowered;
            break;
        }
        case CS_Capacitor: { // 电容器
            bDirectlyPowered = info.mCurrent.mDirection == Facing::UP;
            FacingID dirPower = info.mNearest.mComponent->getDirection();
            // 目前没有已知的dirPower为NOT_DEFINED的情况
            if (dirPower != Facing::NOT_DEFINED)
            {
                CapacitorComponent *capacitor = static_cast<CapacitorComponent *>(info.mNearest.mComponent);
                if (capacitor->getPoweroutDirection() != Facing::NOT_DEFINED)
                    dirPower = capacitor->getPoweroutDirection();

                // 电容器输出端朝向本方块，则升为生产者
                bDirectlyPowered = info.mCurrent.mDirection == dirPower;
                this->mPromotedToProducer = bDirectlyPowered;
            }
            if (info.mCurrent.mDirection == info.mNearest.mComponent->getDirection())
                return false;
            break;
        }
        case CS_Transporter:
            bDirectlyPowered = true;
            break;
        default:
            // 被充能方块搜索到，但是该方块未被充能，则直接返回
            if (id == CS_PoweredBlock && !bPrevDirectlyPowered)
            {
                return false;
            }
            break;
        }
    }
    else if (id == CS_PoweredBlock && !bPrevDirectlyPowered)
    {
        // 不能强充能
        return false;
    }
    return this->trackPowerSource(info, dampening, bDirectlyPowered, id == CS_Consumer) && bDirectlyPowered;
}

bool ConsumerComponent::allowConnection(CircuitSceneGraph &Graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    uint64_t id = info.mCurrent.mTypeID;
    // 判断被搜索元件的类型
    if (id == CS_Capacitor)
    {
        FacingID dirPower = static_cast<CapacitorComponent *>(info.mCurrent.mComponent)->getPoweroutDirection();
        // 正常情况下，没有电容器的PoweroutDirection是NOT_DEFINED
        if (dirPower == Facing::NOT_DEFINED)
        {
            // 此分支不应被执行
            return info.mCurrent.mDirection == 1 && info.mDirectlyPowered;
        }
        else
        {
            // 检查是否对着电容器输入端(比较器中继器的侧面不允许充能方块输入)，info.mDirectlyPowered 此时就是本消费者是否被充能
            return info.mCurrent.mDirection == dirPower && info.mDirectlyPowered;
        }
    }
    else if (id == CS_Transporter && this->mPropagatePower)
    {
        // 红石粉只能被强充能方块激活
        return this->mPromotedToProducer;
    }
    else
    {
        // 其他情况只要本消费者是否被充能即可
        return info.mDirectlyPowered;
    }
}

bool ConsumerComponent::canConsumerPower()
{
    return true;
}

bool ConsumerComponent::canPropagatePower()
{
    return this->mPropagatePower;
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
            // 半脉冲特判
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

uint64_t ConsumerComponent::getBaseType() const
{
    return CS_Consumer;
}

uint64_t ConsumerComponent::getInstanceType() const
{
    return CS_Consumer;
}

bool ConsumerComponent::isPromotedToProducer()
{
    return this->mPromotedToProducer;
}

bool ConsumerComponent::isSecondaryPowered()
{
    return this->mSecondaryPowered;
}

void ConsumerComponent::setAcceptHalfPulse(bool accept)
{
    this->mAcceptHalfPulse = accept;
}

void ConsumerComponent::setPropagatePower(bool propagatePower)
{
    this->mPropagatePower = propagatePower;
}
