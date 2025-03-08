#include "RepeaterCapacitor.h"

#include "deps/Math.h"

void RepeaterCapacitor::delayPulse(RepeaterCapacitor::States States)
{
    for (int i = 0; i < this->mInsertAt; ++i)
    {
        this->mOnStates[i] = this->mOnStates[i + 1];
    }
    for (int i = mInsertAt; i < 4; ++i)
    {
        this->mOnStates[i] = States;
    }
}

int RepeaterCapacitor::getStrength() const
{
    return this->mPowered ? Redstone::SIGNAL_MAX : Redstone::SIGNAL_NONE;
}

bool RepeaterCapacitor::canConsumerPower()
{
    return true;
}

uint64_t RepeaterCapacitor::getInstanceType() const
{
    return CS_Repeater;
}

void RepeaterCapacitor::setDelay(int delay)
{
    this->mInsertAt = mce::Math::clamp(delay, 0, 3);
}

bool RepeaterCapacitor::evaluate(CircuitSystem &system, const BlockPos &pos)
{
    auto oldPowered = this->mPowered;
    if (this->mLocked)
    {
        return false;
    }
    this->delayPulse((RepeaterCapacitor::States)this->mNextPower); // 有缓存输出就传入ON，反之OFF
    if (this->mInsertAt > 0)
    {
        if (this->mPulseCount > this->mInsertAt && (this->mPulse == 0 || this->mPulseCount != 2))
        {
            // 2t周期高频输入特判
            // 特判规则等价表述如下：
            // 1. 对于3或4挡中继器，高频计数大于等于3或4
            // 2. 对于2挡中继器，要么高频计数大于等于3，要么高频计数等于2，且无输入
            this->alternatePulse();
        }
        else
        {
            this->extendPulse();
        }
    }

    this->mPowered = this->mOnStates[0] == States::ON || this->mOnStates[0] == States::ON_LOCKED;
    return oldPowered != this->mPowered;
}

void RepeaterCapacitor::alternatePulse()
{
    States curState = States::OFF;
    if (this->mInsertAt > 0)
    { // 这句if判断了个啥，evaluate那儿不是已经判断过了一次吗？
        curState = this->mOnStates[0];
        if (curState == States::OFF || curState == States::ON) // 若为非锁存状态
        {
            States newStates = this->mNextPulse ? States::ON_LOCKED : States::OFF_LOCKED; // 就变为锁存状态
            this->mNextPulse = !this->mNextPulse;                                         // 并且高频输入类型逻辑取反
            for (int i = 0; i < this->mInsertAt + 1; ++i)
            {
                this->mOnStates[i] = newStates; // 最后更新 mOnStates[] 中的数据，有几挡就更新几个
            }
        }
    }
}
/**
 * @brief 通过一种抽象的逻辑，完成某种摸不着头脑的目的
 */
void RepeaterCapacitor::extendPulse()
{
    if (this->mInsertAt > 0) // 一样，这里必为true
    {
        // 延长锁存信号
        if (this->mOnStates[0] == States::OFF_LOCKED && this->mOnStates[1] == States::OFF)
        {
            this->mOnStates[1] = States::OFF_LOCKED; // 实际上执行到这儿就可以return了
        }
        else if (this->mOnStates[0] == States::ON_LOCKED && this->mOnStates[1] == States::ON)
        {
            this->mOnStates[1] = States::ON_LOCKED; // 这也一样
        }

        States curState = this->mOnStates[0];
        if (curState == States::OFF || curState == States::ON) // 即 mOnStates[0] 为非LOCKED状态
        {
            int iPulse = 0;
            for (int i = 0; i <= this->mInsertAt; ++i)
            {
                if (this->mOnStates[i] == curState)
                {
                    // 统计与 mOnStates[0] 相同的数量，自己也算一个
                    ++iPulse;
                }
            }

            States value;
            if (curState == States::ON)
            {
                value = States::ON_LOCKED;
            }
            else
            {
                value = iPulse <= this->mInsertAt ? States::ON_LOCKED : States::OFF_LOCKED;
            }
            for (int i = 0; i <= this->mInsertAt; ++i)
            {
                this->mOnStates[i] = value;
            }
        }
    }
}

inline FacingID RepeaterCapacitor::getPoweroutDirection() const
{
    return Facing::OPPOSITE_FACING[this->getDirection()];
}

bool RepeaterCapacitor::allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    return info.mCurrent.mDirection == this->getPoweroutDirection();
}

bool RepeaterCapacitor::addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered)
{
    bool forward = this->getDirection() == info.mCurrent.mDirection;                           // 信号到来的方向和中继器方向是否相反
    bool backward = Facing::OPPOSITE_FACING[this->getDirection()] == info.mCurrent.mDirection; // 信号是不是从输入端来的
    if (forward)
        return false; // 从反方向来的直接返回false

    // 如果是充能方块且(充能方块没有信号源 or 充能方块是非直接激活）直接返回
    // 类似这种电路  [红石][铁块][中继器]三个连一起，红石没法搜索到中继器
    if (info.mNearest.mTypeID == CS_PoweredBlock && (!info.mNearest.mComponent->hasDirectPower() || !bDirectlyPowered))
        return false;
    auto type = info.mNearest.mComponent->getInstanceType();
    if (backward || type == CS_Repeater || type == CS_Comparator)
    {
        // 如果上一个元件是比较器或者中继器就两边也能输入（只有中继器和比较器能锁存中继器）
        // 如果不是，只能从输入端输入
        this->trackPowerSource(info, dampening, bDirectlyPowered, backward);
    }

    return false;
}

bool RepeaterCapacitor::removeSource(const BlockPos &posSource, const BaseCircuitComponent *pComponent)
{
    bool bFound = this->removeSource(posSource, pComponent);
    return this->mSideComponents.removeSource(posSource, pComponent) || bFound;
}

void RepeaterCapacitor::updateDependencies(CircuitSceneGraph &system, const BlockPos &pos)
{
    for (auto iter = this->mSources.begin(); iter != this->mSources.end();)
    {
        CircuitComponentList::Item &systemComponent = *iter;
        if (iter->mData == 1)
        {
            iter++;
        }
        else
        {
            this->mSideComponents.add(systemComponent.mComponent, iter->mDampening, iter->mPos);
            iter = this->mSources.erase(iter);
        }
    }
}

void RepeaterCapacitor::setStrength(int strength)
{
    this->mPowered = strength != 0;
    for (int i = 0; i < this->mInsertAt + 1; ++i)
    {
        this->mOnStates[i] = this->mPowered ? States::ON_LOCKED : States::OFF_LOCKED;
    }
    for (int i = this->mInsertAt + 1; i <= 4; ++i)
    {
        this->mOnStates[i] = States::OFF;
    }
}

bool RepeaterCapacitor::consumePowerAnyDirection()
{
    return true;
}

void RepeaterCapacitor::checkLock(CircuitSystem &system, const BlockPos &pos)
{
    this->mLocked = false;
    for (auto &item : this->mSideComponents)
    {
        int currentStrength = std::max(item.mComponent->getStrength() - item.mDampening, 0);
        if (currentStrength > 0)
        {
            this->mLocked = true;
            for (int idx = 1; idx < 4; ++idx)
            {
                this->mOnStates[idx] = this->mOnStates[0];
            }
        }
    }
}

void RepeaterCapacitor::cacheValues(CircuitSystem &system, const BlockPos &pos)
{
    this->mNextPower = 0; // 先检查是否有输入，有则mNextPower置为1，反之置为0
    for (auto &item : this->mSources)
    {
        int inputPower = item.mComponent->getStrength() - item.mDampening;
        inputPower = std::max(inputPower, 0);
        if (inputPower > 0)
        {
            this->mNextPower = 1;
            break;
        }
    }

    if (!this->mLocked)
    {
        if (this->mPulse == this->mNextPower)
        {                          // 输入是否有变化
            this->mPulseCount = 0; // 没变化就mPulseCount为0。
        }
        else
        {
            this->mPulseCount++; // 有变化就mPulseCount自增
            if (this->mPulseCount == 1)
            {                                     // 若自增完后为1
                this->mNextPulse = !this->mPulse; // 则 新的输入 变为与 旧的输入 相反
            }
            this->mPulse = !this->mPulse; // 反转旧的输入，实际上就是同步新旧输入
        }
    }
}