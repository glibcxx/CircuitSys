#pragma once

#include <map>
#include "CircuitComponentList.h"
#include "redstone/Redstone.h"

class CircuitSystem;
class CircuitSceneGraph;
class CircuitTrackingInfo;

class BaseCircuitComponent
{
public:
    BaseCircuitComponent() {}

    virtual ~BaseCircuitComponent()
    {
        for (auto &&iter : this->mSources)
        {
            iter.mComponent->mDestinationList.erase(this);
        }
        for (auto &&iter : this->mDestinationList)
        {
            iter.first->removeFromAnySourceList(this);
        }
    }

    // 获取信号强度，对于信号源，获取输出信号强度
    virtual int getStrength() const { return this->mStrength; }

    // 获取元件方向
    virtual FacingID getDirection() const { return this->mDirection; }

    // 设置元件信号强度，对于信号源，其不一定能修改输出信号强度
    virtual void setStrength(int strength) { this->mStrength = strength; }

    // 设置元件方向
    virtual void setDirection(FacingID direction) { this->mDirection = direction; }

    // 是否存在直接激活的信号源
    bool hasDirectPower();

    virtual void setConsumePowerAnyDirection(bool consume) { this->mConsumePowerAnyDirection = consume; }

    // 是否无条件接收任意方向的信号，这个函数会影响邻接表更新中的搜索行为
    virtual bool canConsumePowerAnyDirection() const { return this->mConsumePowerAnyDirection; }

    // 是否能够接收信号，消费者和电容器会返回true，这个函数不会影响元件在红石系统中的行为
    virtual bool canConsumerPower() const { return false; }

    // 是否需要进行红石方块更新
    bool needUpdating() { return this->mNeedsUpdate; }

    void clearUpdatingFlag() { this->mNeedsUpdate = false; }

    // 似乎所有元件都是false?
    virtual bool canStopPower() const { return false; }

    virtual void setStopPower(bool bPower) {}

    CircuitComponentList mSources{}; // 信号源列表(父元件列表)

    /**
     * @brief 删除一个信号源
     * @param posSource 信号源坐标
     * @param pComponent 信号源裸指针
     */
    virtual void removeSource(const BlockPos &posSource, const BaseCircuitComponent *pComponent);

    /**
     * @brief 添加一个信号源
     * @param graph 元件所在电路图
     * @param info 元件追踪表，`info.mCurrent`代表自己，`info.mNearest`表示搜索到自己的元件
     * @param dampening 阻尼
     * @param bDirectlyPowered 是否直接激活
     * @return 是否需要以本元件为中心继续搜索
     *
     * @details
     * 这个函数以及下面的`allowConnection`函数都与邻接表更新过程耦合。
     * 邻接表更新时从一个信号源开始搜索元件，对于搜索到的每一个新元件，
     * 调用搜索中元件的`allowConnection`检查是否允许与新元件连接，
     * 如何可以，则调用新元件的`addSource`添加信号源。
     * 本函数返回值表示是否需要沿着本元件继续搜索
     */
    virtual bool addSource(CircuitSceneGraph         &graph,
                           const CircuitTrackingInfo &info,
                           int                       &dampening,
                           bool                      &bDirectlyPowered) { return false; }

    /**
     * @brief 检查能否与新搜索到的元件连接
     * @param graph 元件所在电路图
     * @param info 元件追踪表，`info.mCurrent`代表被搜索到的元件，`info.mNearest`表示自己
     * @param bDirectlyPowered 是否直接激活
     * @return bool
     */
    virtual bool allowConnection(CircuitSceneGraph         &graph,
                                 const CircuitTrackingInfo &info,
                                 bool                      &bDirectlyPowered) { return true; }

    /**
     * @brief 检查锁存，仅中继器有效
     * @param system 红石系统
     * @param pos 元件坐标
     */
    virtual void checkLock(CircuitSystem &system, const BlockPos &pos) {}

    /**
     * @brief 红石信号更新的设置信号阶段。
     * @param system 红石系统
     * @param pos 元件坐标
     * @return 元件信号是否有变化
     */
    virtual bool evaluate(CircuitSystem &system, const BlockPos &pos) { return false; }

    /**
     * @brief 红石信号更新的计算(缓存)信号阶段。
     * @param system 红石系统
     * @param pos 元件坐标
     */
    virtual void cacheValues(CircuitSystem &system, const BlockPos &pos) {}

    /**
     * @brief
     * 更新依赖，这个函数用于元件执行一些需要在邻接表更新完成后的操作，否则应在前之前的addSource或allowConnection函数完成。
     * 只有红石火把、中继器、比较器有实际行为
     *
     * @param system 红石系统
     * @param pos 元件坐标
     */
    virtual void updateDependencies(CircuitSceneGraph &system, const BlockPos &pos) {}

    // 弃用
    virtual bool allowIndirect() { return false; }

    void setAllowPowerUp(bool bOnlyPowerUp) { this->mAllowPowerUp = bOnlyPowerUp; }

    /**
     * @brief 这个属性描述压线特性
     * @return
     * true - 红石粉可以斜向上传递，但不能斜向下传递；
     * false - 红石粉可以斜向下传递，但不能斜向上传递
     */
    bool canAllowPowerUp() { return this->mAllowPowerUp; }

    // 弃用
    bool canAllowPowerDown() { return this->mAllowPowerDown; }

    // 弃用
    void setAllowPowerDown(bool bOnlyPowerDown) { this->mAllowPowerDown = bOnlyPowerDown; }

    // 是否输出半脉冲，红石火把相关
    virtual bool isHalfPulse() { return false; }

    /**
     * @brief component是否是当前元件的一个信号源
     * @param source 待检查元件
     * @return bool
     */
    virtual bool hasSource(BaseCircuitComponent &source);

    // 弃用
    virtual bool hasChildrenSource() { return false; }

    void clearFirstTimeFlag() { this->mIsFirstTime = false; }

    /**
     * @brief 是否间接充能
     * @return
     */
    virtual bool isSecondaryPowered() { return false; }

    /**
     * @brief 从所有信号源列表移除component，中继器和比较器有两个信号源列表
     * @param component
     */
    virtual void removeFromAnySourceList(const BaseCircuitComponent *component);

    // 获取元件类型
    virtual CircuitComponentType getCircuitComponentType() const;

    // 获取元件组类型
    virtual CircuitComponentType getCircuitComponentGroupType() const;

    // 标记为已移除
    void setRemoved() { this->mRemoved = true; }

    bool isRemoved() const { return this->mRemoved; }

    /**
     * @brief 添加或更新一个信号源
     * @param info 信号源表项
     */
    void addSourceItem(const CircuitComponentList::Item &info);

    std::map<BaseCircuitComponent *, int> mDestinationList{}; // 子元件列表

    bool     mIgnoreFirstUpdate = false; // 在红石方块更新阶段，忽略首次更新
    bool     mIsFirstTime = true;        // 标记是否是首次更新
    bool     mNeedsUpdate = false;       // 是否需要在红石方块更新阶段更新，在红石信号更新完成后被设为true.
    BlockPos mPos{};                     // 这个值已被弃用
    BlockPos mChunkPosition{};           // 元件所在区块的西北角方块的坐标
    bool     mShouldEvaluate = true;     // 标记是否需要红石信号更新

protected:
    int mStrength = 0; // 信号强度
    /*
     * 元件朝向
     * 中继器、比较器 -> 输入端方向
     * 火把、拉杆 -> 附着方块的方向
     * 活塞 -> 推出方向
     */
    FacingID mDirection = Facing::NOT_DEFINED;

    /**
     * @brief 内部接口，与邻接表更新耦合，添加或更新一个信号源
     * @param info 元件追踪表
     * @param dampening 阻尼
     * @param directlyPowered 是否直接激活
     * @param data 数据值
     * @param duplicates 这个参数会影响元件如何处理重复的信号源
     * @return 是否成功添加新的或更新已有的信号源
     */
    bool trackPowerSource(const CircuitTrackingInfo &info, int dampening, bool directlyPowered, int data, bool duplicates = false);

    // 弃用
    bool calculateValue(CircuitSystem &system);

    bool mAllowPowerUp = false;  // 见 `canAllowPowerUp`函数
    bool mAllowPowerDown = true; // 弃用

private:
    bool                 mRemoved = false;          // 标记是否被移除
    bool                 mConsumePowerAnyDirection; // 见 `canConsumePowerAnyDirection` 函数
    CircuitComponentType mCircuitComponentType;     // 元件类型
};
