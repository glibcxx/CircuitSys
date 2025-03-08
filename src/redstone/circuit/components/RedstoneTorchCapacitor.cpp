#include "RedstoneTorchCapacitor.h"

#include "redstone/circuit/CircuitSystem.h"
#include "Facing.h"


bool isQueueBeingPowered(CircuitComponentList &sources);


bool findRepeaterScenario(CircuitComponentList &sources, CircuitSceneGraph &system);


void PushCircularReference(std::unordered_map<BlockPos, RedstoneTorchCapacitor *> &relatedTorches, const BlockPos &pos, std::queue<RedstoneTorchCapacitor *> &list);

int RedstoneTorchCapacitor::getStrength() const
{
    return this->mState[0].mOn ? Redstone::SIGNAL_MAX : Redstone::SIGNAL_NONE;
}

uint64_t RedstoneTorchCapacitor::getInstanceType() const
{
    return CS_Torch;
}

bool RedstoneTorchCapacitor::removeSource(const BlockPos &posSource, const BaseCircuitComponent *pComponent)
{
    bool bRemoved = this->BaseCircuitComponent::removeSource(posSource, pComponent);
    if (this->mNextOrder != pComponent || !this->mNextOrder)
        return bRemoved;
    this->mNextOrder = this->mNextOrder->mNextOrder;
    return 1;
}

bool RedstoneTorchCapacitor::addSource(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, int &dampening, bool &bDirectlyPowered)
{
    uint64_t id = info.mNearest.mTypeID;
    int      direction = info.mCurrent.mDirection;
    if (id != CS_PoweredBlock && id != CS_Transporter && id != CS_Consumer && direction == Facing::OPPOSITE_FACING[info.mCurrent.mComponent->getDirection()])
    { // 生产者或电容器
        this->BaseCircuitComponent::trackPowerSource(info, dampening, bDirectlyPowered, 0);
    }
    return false;
}

bool RedstoneTorchCapacitor::allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    bool                bAttached = this->getDirection() == info.mCurrent.mDirection;
    CircuitTrackingInfo associatedInfo(info);
    associatedInfo.mPower.mPos = info.mCurrent.mPos;
    associatedInfo.mPower.mComponent = info.mCurrent.mComponent;
    this->trackPowerSource(associatedInfo, info.mDampening, info.mDirectlyPowered, 0);
    if (((info.mCurrent.mTypeID == CS_PoweredBlock || info.mCurrent.mTypeID == CS_Producer) && info.mCurrent.mDirection == info.mNearest.mDirection || info.mCurrent.mTypeID == CS_Consumer && bAttached) && (info.mCurrent.mTypeID == CS_PoweredBlock))
    {
        return false;
    }
    else if (!info.mCurrent.mDirection)
        return this->mDirection != 0;
    else
        return !(this->mDirection && this->mDirection == 0);
}

bool RedstoneTorchCapacitor::evaluate(CircuitSystem &system, const BlockPos &pos)
{
    this->mState[0] = this->mState[1];
    if (this->mNextOrder && this->_canIncrementSelfPower())
        ++this->mSelfPowerCount;
    return this->mState[0].mChanged;
}

void RedstoneTorchCapacitor::cacheValues(CircuitSystem &system, const BlockPos &pos)
{
    if (this->mNextOrder)
    {
        this->mState[1].mHalfFrame = false;
        if (isQueueBeingPowered(this->mSources))
        { // 有非火把作信号源
            this->mState[1].mOn = true;
            this->mState[1].mChanged = !this->mState[0].mOn;
            this->mSelfPowerCount = -1;
        }
        else if (this->mSelfPowerCount)
        {
            this->mState[1].mOn = false;
            this->mState[1].mChanged = this->mState[0].mOn;
        }
        else
        { // mSelfPowerCount == 0
            this->mState[1].mOn = true;
            this->mState[1].mChanged = !this->mState[0].mOn;
            this->mNextOrder->mSelfPowerCount = -1;
            RedstoneTorchCapacitor *others = this->mNextOrder->mNextOrder;
            for (int count = 0; count < 4 && others != nullptr && others != this; ++count)
            {
                others->mSelfPowerCount = 1;
                others = others->mNextOrder;
            }
        }
    }
    else
    {
        bool bSelfPowered = 0;
        bool bHalfFrame;
        bool onState;
        int  newStrength = this->FindStrongestStrength(pos, system, bSelfPowered);

        // 如果是自激活
        if (bSelfPowered)
        {
            if (this->_canIncrementSelfPower())
                ++this->mSelfPowerCount; // 计数+1，最高33

            // 计数小于16，就是半脉冲高频；大于等于16，就熄灭，但是等于16时还是有最后一个烧火把信号
            if (this->mSelfPowerCount >= 16)
            {
                onState = 0;
                bHalfFrame = this->mSelfPowerCount == 16;
            }
            else
            {
                onState = !this->mState[0].mOn;
                bHalfFrame = 1;
            }
        }
        else
        {
            this->mSelfPowerCount = 0;
            this->mCanReigniteFromBurnout = 0;
            onState = newStrength <= 0;
            bHalfFrame = 0;
        }
        bool bChange = onState != this->mState[0].mOn;
        this->mState[1].mHalfFrame = bHalfFrame;
        this->mState[1].mOn = onState;
        this->mState[1].mChanged = bChange;
        if (this->mSelfPowerCount > 32)
            this->mCanReigniteFromBurnout = 1;
    }
}

void RedstoneTorchCapacitor::updateDependencies(CircuitSceneGraph &system, const BlockPos &pos)
{
    if (!findRepeaterScenario(this->mSources, system))
        this->mNextOrder = nullptr;
}

bool RedstoneTorchCapacitor::isHalfPulse()
{
    return this->mState[0].mHalfFrame;
}

FacingID RedstoneTorchCapacitor::getPoweroutDirection() const
{
    return Facing::UP;
}

int RedstoneTorchCapacitor::getSelfPowerCount()
{
    return this->mSelfPowerCount;
}

/**
 * 获取最大信号源的值，检查自激活
 * @param targetPos 自己的坐标
 * @param system 元件所在红石系统
 * @param bPowerFromSelf 是否为自激活
 * @return 最大信号值；bPowerFromSelf 的值也会被更新
 */
int RedstoneTorchCapacitor::FindStrongestStrength(const BlockPos &targetPos, CircuitSystem &system, bool &bPowerFromSelf)
{
    // if加for最高8层嵌套，套得我心慌。。。
    bPowerFromSelf = 0;
    int newStrength = 0;
    if (this->mSources.size() > 0)
    {
        CircuitComponentList::Item *pFoundEntry = nullptr; // 存储信号源的指针
        CircuitComponentList::Item *pSelfEntry = nullptr;  // 信号源是自己
        CircuitComponentList::Item *pNotSelfEntry = nullptr;
        for (auto &entry : this->mSources) // 遍历信号源
        {
            if (!entry.mComponent)
            {
                continue;
            }

            uint64_t searchID = entry.mComponent->getBaseType();
            // 生产者，或者有信号源的电容器
            if (searchID == CS_Producer || searchID == CS_Capacitor && !entry.mComponent->mSources.size())
            {
                int curStrength = entry.mComponent->getStrength();
                if (curStrength >= newStrength && curStrength)
                {
                    pFoundEntry = &entry;
                    newStrength = curStrength;
                }
            }
            else // 消费者，充能方块
            {
                int indirectValue = 0; // (最大)信号值
                int selfValue = 0;     // 自己激活自己的信号值
                for (auto searchEntry : entry.mComponent->mSources)
                { // 遍历消费者、充能方块的信号源
                    // 直接激活
                    if (searchEntry.mDirectlyPowered && searchEntry.mComponent)
                    {
                        indirectValue = std::max(searchEntry.mComponent->getStrength() - searchEntry.mDampening, 0);
                        if (indirectValue >= newStrength && indirectValue != 0)
                        {
                            newStrength = indirectValue;
                            pFoundEntry = &searchEntry;
                            if (targetPos != searchEntry.mPos)
                                pNotSelfEntry = &searchEntry;
                        }
                        if (targetPos == searchEntry.mPos)
                        { // 信号源是火把自己
                            pSelfEntry = &searchEntry;
                            selfValue = indirectValue;
                        }
                    }
                }

                // 且最大信号值等于自己激活自己的信号值，但是提供最大信号的元件不是自己
                if (indirectValue == selfValue && pNotSelfEntry)
                {
                    pFoundEntry = pNotSelfEntry;
                    pSelfEntry = nullptr;
                }
                else if (!newStrength && pSelfEntry) // 最大信号源就是自己
                {
                    pFoundEntry = pSelfEntry;
                }
                bPowerFromSelf = pFoundEntry == pSelfEntry && pSelfEntry != nullptr;
            }
        }
    }
    return newStrength;
}

bool RedstoneTorchCapacitor::_canIncrementSelfPower()
{
    return !this->mCanReigniteFromBurnout && this->mSelfPowerCount <= 32;
}

void RedstoneTorchCapacitor::setOn(bool bOn)
{
    this->mState[0].mOn = bOn;
    this->mState[0].mChanged = true;
    this->mState[0].mHalfFrame = false;
    this->mSelfPowerCount = false;
    this->mCanReigniteFromBurnout = false;
    this->mNextOrder = nullptr;
}

void RedstoneTorchCapacitor::resetBurnOutCount()
{
    if (this->mCanReigniteFromBurnout)
    {
        this->mSelfPowerCount = 0;
        this->mCanReigniteFromBurnout = 0;
    }
}

void RedstoneTorchCapacitor::setSelfPowerCount(int count)
{
    this->mSelfPowerCount = count;
}

void RedstoneTorchCapacitor::setNextInQueue(RedstoneTorchCapacitor *torch)
{
    this->mNextOrder = torch;
}

/**
 * 若存在非红石火把激活，返回true
 */
bool isQueueBeingPowered(CircuitComponentList &sources)
{
    if (sources.size() == 0)
    {
        return false;
    }
    for (auto &item : sources)
    { // 遍历父元件
        if (!item.mComponent)
        { // 判断空指针
            return false;
        }
        uint64_t searchID = item.mComponent->getBaseType();
        if (searchID == CS_PoweredBlock || searchID == CS_Consumer)
        { // 若父元件为充能方块或消费者
            for (auto &source : item.mComponent->mSources)
            { // 遍历充能方块或消费者的父元件
                auto type = source.mComponent->getInstanceType();
                if (source.mDirectlyPowered && source.mComponent && type != CS_Torch && std::max(source.mComponent->getStrength() - source.mDampening, 0))
                {
                    // 若由非红石火把充能，则返回true
                    return true;
                }
            }
        }
    }
    return false;
}

bool findRepeaterScenario(CircuitComponentList &sources, CircuitSceneGraph &system)
{
    bool bFound = false;
    for (auto &entry : sources)
    { // 遍历火把的父元件
        if (!entry.mComponent)
        {
            return false;
        }
        BlockPos targetUpPos = entry.mPos + BlockPos(0, 1, 0);
        uint64_t searchID = entry.mComponent->getBaseType();

        // 父元件上面有传输者(红石粉)，父元件是充能方块或消费者
        if (system.getComponent(targetUpPos, CS_Transporter) && (searchID == CS_PoweredBlock || searchID == CS_Consumer))
        {
            // 寻找父元件的火把信号源，且该火把也是插在这个父元件的，放入relatedTorches中
            std::unordered_map<BlockPos, RedstoneTorchCapacitor *> relatedTorches;
            for (auto &searchEntry : entry.mComponent->mSources)
            {
                if (searchEntry.mDirectlyPowered && searchEntry.mComponent)
                { // 遍历信号源
                    auto     direction = searchEntry.mComponent->getDirection();
                    BlockPos powerBlockTarget = searchEntry.mPos + Facing::DIRECTION[direction];
                    if (powerBlockTarget == entry.mPos && searchEntry.mComponent->getInstanceType() == CS_Torch)
                    { // 信号源是火把，插在前面的父元件上
                        BlockPos                searchUpPos = searchEntry.mPos + BlockPos(0, 1, 0);
                        RedstoneTorchCapacitor *torch = (RedstoneTorchCapacitor *)searchEntry.mComponent;
                        // 信号源火把的上方是充能方块或消费者
                        if ((system.getComponent(searchUpPos, CS_PoweredBlock) || system.getComponent(searchUpPos, CS_Consumer)) && torch->getDirection())
                        {
                            relatedTorches.insert({searchEntry.mPos, torch});
                        }
                    }
                }
            }
            if (relatedTorches.size() > 1)
            {
                std::queue<RedstoneTorchCapacitor *> list;
                // relatedTorches内的火把一定是插在同一个方块上的，且最多4根
                // 将它们按北、西、南、东的顺序添加到list
                BlockPos northPos = entry.mPos + BlockPos(0, 0, -1);
                BlockPos westPos = entry.mPos + BlockPos(-1, 0, 0);
                BlockPos southPos = entry.mPos + BlockPos(0, 0, 1);
                BlockPos eastPos = entry.mPos + BlockPos(1, 0, 0);
                PushCircularReference(relatedTorches, northPos, list);
                PushCircularReference(relatedTorches, westPos, list);
                PushCircularReference(relatedTorches, southPos, list);
                PushCircularReference(relatedTorches, eastPos, list);

                // 将火把的 NextOrder，按 北->西->南->东->北 的顺序串起来
                // 队列list第一个火把自激活计数置0，最后一个置16
                if (list.size())
                {
                    RedstoneTorchCapacitor *front = list.front();
                    front->setSelfPowerCount(0);
                    RedstoneTorchCapacitor *back = list.back();
                    back->setNextInQueue(front);
                    back->setSelfPowerCount(0);
                    while (list.size() > 1)
                    {
                        RedstoneTorchCapacitor *current = list.front();
                        list.pop();
                        RedstoneTorchCapacitor *next = list.front();
                        current->setNextInQueue(next);
                        back->setSelfPowerCount(16);
                    }
                }
                bFound = true;
            }
        }
    }
    return bFound;
}

void PushCircularReference(std::unordered_map<BlockPos, RedstoneTorchCapacitor *> &relatedTorches, const BlockPos &pos, std::queue<RedstoneTorchCapacitor *> &list)
{
    auto iteration = relatedTorches.find(pos);
    if (iteration != relatedTorches.end())
    {
        list.push(iteration->second);
    }
}