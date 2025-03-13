#pragma once

#include <vector>
#include <memory>
#include <gsl/gsl>
#include "CircuitSceneGraph.h"
#include "level/BlockSource.h"

/**
 * @brief 红石系统，外部有关红石系统的一切操作都由本类提供接口。其内部维护着一张电路图。
 */
class CircuitSystem
{
public:
    CircuitSystem() {}

    // 红石系统信号更新的入口
    void evaluate(BlockSource *region);

    /**
     * @brief 红石方块更新的入口，用于更新一个区块内的红石元件
     * @param chunkPos 区块的坐标
     */
    void updateBlocks(BlockSource &region, const BlockPos &chunkPos);

    // 红石依赖更新的入口，用于更新电路图
    void updateDependencies(BlockSource *region);

private:
    // 确定所有活跃元件是否需要信号更新，当元件未被移除且所在区块已加载的情况下，元件需要信号更新
    void _shouldEvaluate(BlockSource *region);

    /**
     * @brief 信号更新的设置信号阶段，生产者和消费者分开
     * @param bOnlyProducers true仅设置生产者和电容器，false仅设置消费者
     */
    void evaluateComponents(bool bOnlyProducers);

    // 信号更新的计算信号阶段
    void cacheValues();

    // 刷新所有中继器锁存状态
    void checkLocks();

    /**
     * @brief 红石方块更新阶段中，用于更新单个元件的函数，通过调用 `Block::onRedstoneUpdate` 触发元件行为
     * @param component 元件
     * @param chunkPos 元件所在区块的坐标
     * @param pos 元件坐标
     * @param region bs
     */
    void updateIndividualBlock(gsl::not_null<BaseCircuitComponent *> component,
                               const BlockPos                       &chunkPos,
                               const BlockPos                       &pos,
                               BlockSource                          &region);

public:
    /**
     * @brief 创建一个元件
     * @tparam ComponentClass 元件类
     * @param pos 坐标
     * @param pSource bs
     * @param type 元件类型
     * @param direction 方向
     * @return 元件指针
     */
    template <typename ComponentClass>
    ComponentClass *create(const BlockPos &pos, BlockSource *pSource, CircuitComponentType type, FacingID direction)
    {
        if (this->mLockGraph)
            return this->getComponent<ComponentClass>(pos, type);
        std::unique_ptr<BaseCircuitComponent> newComponent = std::make_unique<ComponentClass>();
        BaseCircuitComponent                 *Component = this->createComponent(pos, direction, std::move(newComponent));
        if (pSource)
        {
            Component->mChunkPosition = pSource->getPlaceChunkPos();
        }
        return static_cast<ComponentClass *>(Component);
    }

    /**
     * @brief 尝试移除某个位置的元件
     * @param pos 坐标
     */
    void removeComponents(const BlockPos &pos);

private:
    /**
     * @brief 内部接口，用于把创建的元件添加到电路图中
     * @param pos 元件坐标
     * @param direction 元件朝向
     * @param newComponent 元件独占指针
     * @return 元件裸指针
     */
    BaseCircuitComponent *createComponent(const BlockPos &pos, FacingID direction, std::unique_ptr<BaseCircuitComponent> newComponent);

public:
    static int const NOT_DEFINED_STRENGTH = -1;

    /**
     * @brief 设置某个元件的信号强度，注意这个信号强度不一定代表输出的信号强度，它只是红石元件的一个通用属性
     * @param pos 坐标
     * @param strength 强度
     */
    void setStrength(const BlockPos &pos, int strength);

    /**
     * @brief 获取某个元件的信号强度
     * @param pos 坐标
     * @return 信号强度
     */
    int getStrength(const BlockPos &pos);

    /**
     * @brief 获取某个元件的方向，不同元件的方向有不同的定义
     * @param pos 坐标
     * @return 朝向id
     */
    FacingID getDirection(const BlockPos &pos);

    /**
     * @brief 是否有一个直接激活的信号源
     * @param pos
     * @return
     */
    bool hasDirectPower(const BlockPos &pos);

    /**
     * @brief 按位置查找元件
     * @tparam ComponentClass
     * @param pos 坐标
     * @param type 元件类型
     * @return 元件指针，未找到返回`nullptr`
     */
    template <typename ComponentClass>
    ComponentClass *getComponent(const BlockPos &pos, CircuitComponentType type)
    {
        ComponentClass *component = static_cast<ComponentClass *>(this->mSceneGraph.getComponent(pos, type));
        if (component)
            return component;
        else
            return static_cast<ComponentClass *>(this->mSceneGraph.getFromPendingAdd(pos, type));
    }

    /**
     * @brief 检查指定位置是否有红石元件
     * @param pos 坐标
     * @return bool
     */
    bool isAvailableAt(const BlockPos &pos);

    /**
     * @brief 检查`component`是否可以与pos的face方向处的元件连接
     * @param component 元件
     * @param pos 元件的坐标
     * @param face 方向
     * @param bDirectlyPowered 直接激活
     * @return bool
     */
    bool isConnectionAllowed(BaseCircuitComponent &component, const BlockPos &pos, FacingID face, bool bDirectlyPowered);

public:
    /**
     * @brief 计划pos处元件的邻接表更新
     * @param pos 坐标
     */
    void invalidatePos(const BlockPos &pos);

    /**
     * @brief 用于新加载区块时，创建充能方块。原因是有时邻接表更新触发在一个未被加载的区块，此时需要在区块加载后重新进行一次邻接表更新
     * @param chunkPos 区块坐标
     */
    void preSetupPoweredBlocks(const ChunkPos &chunkPos);

    /**
     * @brief 更新电路图锁住状态
     * @param bLock 是否锁住
     */
    void lockGraph(bool bLock);

private:
    bool mLockGraph = false; // 电路图是否锁住

    CircuitSceneGraph mSceneGraph{}; // 电路图

    /**
     * @brief 方块坐标转区块西北角方块的坐标
     * @param region bs
     * @param pos 方块坐标
     * @return 区块西北角方块的坐标
     */
    const BlockPos getChunkPos(BlockSource *region, const BlockPos &pos);

    class LevelChunkTracking
    {
    public:
        LevelChunkTracking(BlockPos pos);

        BlockPos mChunkPos;
    };

    std::vector<LevelChunkTracking> mAddedLevelChunk;

private:
    bool mHasBeenEvaluated = true; // 是否完成红石信号更新
};
