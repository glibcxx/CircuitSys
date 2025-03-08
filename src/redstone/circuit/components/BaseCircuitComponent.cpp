#include "BaseCircuitComponent.h"
#include "redstone/circuit/CircuitSystem.h"
#include "redstone/circuit/CircuitTrackingInfo.h"
#include "deps/Math.h"

/**
 * 计算当前元件信号强度的基本算法
 * @param system
 * @return
 */
bool BaseCircuitComponent::calculateValue(CircuitSystem &system)
{
    int newStrength = 0;
    // 遍历所有红石元件
    // 计算信号源强度 - 电阻 并取最大值
    for (auto &source : this->mSources.mComponents)
    {
        auto currentStrength = source.mComponent->getStrength() - source.mDampening;
        // 如果信号源是半脉冲信号，就强制用15作为该信号源的强度
        if (source.mComponent->isHalfPulse())
            currentStrength = Redstone::SIGNAL_MAX - source.mDampening;
        if (newStrength < currentStrength)
            newStrength = currentStrength;
    }

    bool hasChanged = this->mStrength != newStrength;
    this->mStrength = newStrength;
    return hasChanged || this->mIsFirstTime && this->mStrength == 0;
}

void BaseCircuitComponent::clearFirstTimeFlag()
{
    this->mIsFirstTime = false;
}

/*
 * 这个函数是被信号源搜索到并准备添加新的信号源的时候执行对当前信号源的相关信息进行修正,
 * data是描述连接的一种情况，比如中继器的描述的是 是从输入端进行输入还是锁存
 */
bool BaseCircuitComponent::trackPowerSource(const CircuitTrackingInfo &info, int dampening, bool directlyPowered, int data)
{
    for (auto &iter : this->mSources)
    {
        if (iter.mPos == info.mPower.mPos)
        {
            if (iter.mDirectlyPowered || !directlyPowered) // 已有的信号源信息为直接激活，或者虽然不是直接激活但是新的信号源信息也不是
                                                           // 就只需更新一下mDampening就行了
            {
                if (iter.mDampening - 1 <= info.mDampening - 1) // 看来天才mojang程序员把自己绕晕了，/*iter.mDampening <= dampening - 1*/
                    return false;                               // 这里包括下面不应该用info.mDampening，而是用函数形参dampening
                else
                    iter.mDampening = info.mDampening - 1 < 0 ? 0 : info.mDampening - 1;
                return true;
            }
            else // 否则mDirectlyPowered、mDampening都要更新
            {
                iter.mDirectlyPowered = true;
                if (info.mPower.mComponent && info.mPower.mComponent->getStrength() <= info.mDampening - 1)
                    iter.mDampening = mce::Math::max(mce::Math::min(info.mDampening - 1, iter.mDampening), 0);
                else
                    iter.mDampening = info.mDampening - 1 < 0 ? 0 : info.mDampening - 1;

                return true;
            }
        }
    }
    CircuitComponentList::Item value;
    value.mDampening = dampening - 1 < 0 ? 0 : dampening - 1;
    value.mPos = info.mPower.mPos;
    value.mDirection = info.mCurrent.mDirection;
    value.mDirectlyPowered = directlyPowered;
    value.mComponent = info.mPower.mComponent;
    value.mData = data;
    this->mSources.push_back(value);
    return true;
}

/*
 * 允许搜索的过程中元件A能搜索到元件B
 */
bool BaseCircuitComponent::allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    return true;
}

void BaseCircuitComponent::checkLock(CircuitSystem &, const BlockPos &) {}

bool BaseCircuitComponent::evaluate(CircuitSystem &, const BlockPos &)
{
    return false;
}

void BaseCircuitComponent::cacheValues(CircuitSystem &, const BlockPos &pos) {}

void BaseCircuitComponent::updateDependencies(CircuitSceneGraph &graph, const BlockPos &pos) {}

bool BaseCircuitComponent::allowIndirect()
{
    return false;
}

bool BaseCircuitComponent::trackPowerSourceDuplicates(const CircuitTrackingInfo &info, int dampening, bool directlyPowered)
{
    for (auto &&iter : this->mSources)
    {
        if (iter.mPos != info.mPower.mPos || iter.mDirection != info.mCurrent.mDirection)
            continue;

        if (iter.mDirectlyPowered || !directlyPowered)
        {
            if (iter.mDampening - 1 <= info.mDampening - 1)
                return false;
            else
                iter.mDampening = info.mDampening - 1 >= 0 ? info.mDampening - 1 : 0;
            return true;
        }
        else
        {
            iter.mDirectlyPowered = 1;
            iter.mDampening = info.mDampening - 1 >= 0 ? info.mDampening - 1 : 0;
            return true;
        }
    }
    CircuitComponentList::Item value;
    value.mDampening = dampening - 1 >= 0 ? dampening - 1 : 0;
    value.mPos = info.mPower.mPos;
    value.mDirection = info.mCurrent.mDirection;
    value.mDirectlyPowered = directlyPowered;
    value.mComponent = info.mPower.mComponent;
    this->mSources.push_back(value);
    return true;
}

bool BaseCircuitComponent::canAllowPowerUp()
{
    return this->mAllowPowerUp;
}

bool BaseCircuitComponent::canAllowPowerDown()
{
    return this->mAllowPowerDown;
}

void BaseCircuitComponent::setAllowPowerDown(bool bOnlyPowerDown)
{
    this->mAllowPowerDown = bOnlyPowerDown;
}

void BaseCircuitComponent::setAllowPowerUp(bool bOnlyPowerUp)
{
    this->mAllowPowerUp = bOnlyPowerUp;
}

void BaseCircuitComponent::setRemoved()
{
    this->mRemoved = true;
}

bool BaseCircuitComponent::isRemoved() const
{
    return this->mRemoved;
}

int BaseCircuitComponent::getStrength() const
{
    return this->mStrength;
}

FacingID BaseCircuitComponent::getDirection() const
{
    return this->mDirection;
}

void BaseCircuitComponent::setStrength(int strength)
{
    this->mStrength = strength;
}

void BaseCircuitComponent::setDirection(FacingID facing)
{
    this->mDirection = facing;
}

bool BaseCircuitComponent::consumePowerAnyDirection()
{
    return false;
}

bool BaseCircuitComponent::canConsumerPower()
{
    return false;
}

bool BaseCircuitComponent::needUpdating()
{
    return this->mNeedsUpdate;
}

void BaseCircuitComponent::clearUpdatingFlag()
{
    this->mNeedsUpdate = false;
}

bool BaseCircuitComponent::canStopPower()
{
    return false;
}

void BaseCircuitComponent::setStopPower(bool bPower) {}

uint64_t BaseCircuitComponent::getBaseType() const
{
    return CS_BaseCircuit;
}

uint64_t BaseCircuitComponent::getInstanceType() const
{
    return CS_BaseCircuit;
}

/**
 * 为当前元件删除一个信号源
 * @param pos
 * @param component
 */
bool BaseCircuitComponent::removeSource(const BlockPos &pos, const BaseCircuitComponent *component)
{
    return this->mSources.removeSource(pos, component);
}

bool BaseCircuitComponent::addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered)
{
    return false;
}

bool BaseCircuitComponent::isHalfPulse()
{
    return false;
}

/*
 * `component`是否是当前元件的一个信号源
 */
bool BaseCircuitComponent::hasSource(BaseCircuitComponent &component)
{
    if (this->mSources.size() <= 0)
        return false;
    for (auto &systemComponent : mSources.mComponents)
    {
        if (systemComponent.mComponent == &component)
            return true;
        // 信号源的信号源也算(前提是新仓元有子信号源)
        if (systemComponent.mComponent->hasChildrenSource() && systemComponent.mComponent->hasSource(component))
            return true;
    }
    return false;
}

bool BaseCircuitComponent::hasChildrenSource()
{
    return false;
}

bool BaseCircuitComponent::isSecondaryPowered()
{
    return false;
}

/**
 * 是否有一个直接激活的信号源
 * @return
 */
bool BaseCircuitComponent::hasDirectPower()
{
    for (auto &s : this->mSources)
    {
        if (s.mDirectlyPowered)
            return true;
    }
    return false;
}
