#pragma once

#include <unordered_map>
#include <memory>
#include <queue>
#include "components/BaseCircuitComponent.h"

class BlockSource;

/**
 * @brief 电路图
 */
class CircuitSceneGraph
{
public:
    typedef std::unordered_map<BlockPos, std::unique_ptr<BaseCircuitComponent>> ComponentMap;
    typedef std::unordered_map<BlockPos, CircuitComponentList>                  ComponentsPerPosMap;

    CircuitSceneGraph() {}

    ~CircuitSceneGraph() {}

    /**
     * @brief 红石依赖更新。包含 处理待移除、处理待添加、处理待更新 三个阶段，其中邻接表更新主要由第三个阶段完成。
     * @param region
     */
    void update(BlockSource *region);

    /**
     * @brief 向电路图添加一个元件（添加到待添加队列）
     * @param pos 元件坐标
     * @param component 元件的独占指针
     */
    void add(const BlockPos &pos, std::unique_ptr<BaseCircuitComponent> component);

    /**
     * @brief 从电路图中移除一个元件（添加到待移除队列）
     * @param pos 坐标
     * @param component 裸指针
     */
    void remove(const BlockPos &pos, BaseCircuitComponent *component);

    /**
     * @brief pos坐标是否处于待添加队列中
     * @param pos 坐标
     * @return bool
     */
    bool isPendingAdd(const BlockPos &pos);

    /**
     * @brief 从待添加队列中获取指定类型的元件
     * @param pos 坐标
     * @param typeID 元件类型
     * @return 元件裸指针
     */
    BaseCircuitComponent *getFromPendingAdd(const BlockPos &pos, CircuitComponentType typeID);

    /**
     * @brief 从待添加队列中查找元件
     * @param pos 坐标
     * @return 元件裸指针
     */
    BaseCircuitComponent *getFromPendingAdd(const BlockPos &pos);

    void setPendingAddAsNewlyLoaded();

    /**
     * @brief 从电路图中获取指定类型的元件
     * @param pos 元件坐标
     * @param typeID 元件类型
     * @return 元件裸指针
     */
    BaseCircuitComponent *getComponent(const BlockPos &pos, CircuitComponentType typeID);

    /**
     * @brief 从电路图中获取元件
     * @param pos 元件坐标
     * @return 元件裸指针
     */
    BaseCircuitComponent *getBaseComponent(const BlockPos &pos);

    /**
     * @brief 获取全局元件表
     * @return [坐标->元件指针] 哈希表
     */
    ComponentMap &getComponents_FastLookupByPos();

    /**
     * @brief 获取活跃元件表
     * @return vector 容器
     */
    CircuitComponentList &getComponents_FastIterationAcrossActive();

    /**
     * @brief 获取区块活跃元件表
     * @return [区块的角落方块坐标->元件列表] 哈希表
     */
    ComponentsPerPosMap &getComponents_FastLookupByChunkPos();

private:
    /**
     * @brief 红石系统信号更新的处理 待更新 阶段
     * @param region bs
     */
    void processPendingUpdates(BlockSource *region);

    /**
     * @brief 红石系统信号更新的处理 待添加 阶段
     */
    void processPendingAdds();

    /**
     * @brief 红石系统信号更新的处理 待移除 阶段
     */
    void processPendingRemoves();

    /**
     * @brief 将元件加入待更新列表
     * @param pos 元件坐标
     * @param component 元件裸指针
     */
    void scheduleRelationshipUpdate(const BlockPos &pos, BaseCircuitComponent *component);

    /**
     * @brief 从producerTarget开始，搜索子元件，更新连接关系
     * @param pos producerTarget的坐标
     * @param producerTarget 一个信号源
     * @param region bs
     */
    void findRelationships(const BlockPos &pos, BaseCircuitComponent *producerTarget, BlockSource *region);

    /**
     * @brief 清除所有元件的连接关系
     */
    void removeStaleRelationships();

    /**
     * @brief 从电路图中彻底移除一个元件
     * @param pos 元件坐标
     */
    void removeComponent(const BlockPos &pos);

public:
    /**
     * @brief 尝试添加一个充能方块
     * @param source bs
     * @param pos 方块坐标
     * @return 添加成功则返回元件指针，反之 nullptr
     */
    BaseCircuitComponent *addIfPoweredBlockAt(BlockSource &source, const BlockPos &pos);

    /**
     * @brief 计划邻接表更新，如果pos处是红石元件，将其所有信号源添加到待更新列表，反之尝试将周围6个面的元件的信号源添加到待更新列表
     * @param pos 坐标
     */
    void invalidatePos(const BlockPos &pos);

    /**
     * @brief 用于新加载区块时，创建充能方块。原因是有时邻接表更新触发在一个未被加载的区块，此时需要在区块加载后重新进行一次邻接表更新
     * @param chunkPos 区块坐标
     */
    void preSetupPoweredBlocks(const ChunkPos &chunkPos);

    /**
     * @brief 添加到待重新更新列表
     * @param chunkPos
     * @param pos
     */
    void addPositionToReEvaluate(const ChunkPos &chunkPos, const BlockPos &pos);

private:
    ComponentMap         mAllComponents{};            // 全局元件表
    CircuitComponentList mActiveComponents{};         // 活跃元件表
    ComponentsPerPosMap  mActiveComponentsPerChunk{}; // 区块活跃元件表
    ComponentsPerPosMap  mPowerAssociationMap{};      // 邻接表

    class PendingEntry
    {
    public:
        PendingEntry(CircuitSceneGraph::PendingEntry &&entry);

        PendingEntry(const BlockPos &_pos, std::unique_ptr<BaseCircuitComponent> _component);

        PendingEntry(const BlockPos &_pos, BaseCircuitComponent *_component);

        PendingEntry &operator=(CircuitSceneGraph::PendingEntry &&rhs);

        BaseCircuitComponent                 *mRawComponentPtr;
        std::unique_ptr<BaseCircuitComponent> mComponent;
        BlockPos                              mPos;
    };

    std::unordered_map<BlockPos, CircuitSceneGraph::PendingEntry> mPendingAdds{};            // 待添加队列
    std::unordered_map<BlockPos, CircuitSceneGraph::PendingEntry> mPendingUpdates{};         // 待更新队列
    std::unordered_map<BlockPos, std::vector<BlockPos>>           mComponentsToReEvaluate{}; // 待重新更新队列
    std::vector<CircuitSceneGraph::PendingEntry>                  mPendingRemoves{};         // 待移除队列
};