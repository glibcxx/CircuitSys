#pragma once

#include <unordered_map>
#include <memory>
#include <queue>
#include "components/BaseCircuitComponent.h"

class BlockSource;

class CircuitSceneGraph
{
public:
    typedef std::unordered_map<BlockPos, std::unique_ptr<BaseCircuitComponent>> ComponentMap;
    typedef std::unordered_map<BlockPos, CircuitComponentList>                  ComponentsPerPosMap;

    CircuitSceneGraph() {}

    ~CircuitSceneGraph() {}

    void update(BlockSource *region);

    void add(const BlockPos &pos, std::unique_ptr<BaseCircuitComponent> component);

    void remove(const BlockPos &pos, BaseCircuitComponent *component);

    bool isPendingAdd(const BlockPos &pos);

    BaseCircuitComponent *getFromPendingAdd(const BlockPos &pos, CircuitComponentType typeID);

    BaseCircuitComponent *getFromPendingAdd(const BlockPos &pos);

    void setPendingAddAsNewlyLoaded();

    BaseCircuitComponent *getComponent(const BlockPos &pos, CircuitComponentType typeID);

    BaseCircuitComponent *getBaseComponent(const BlockPos &pos);

    ComponentMap &getComponents_FastLookupByPos();

    CircuitComponentList &getComponents_FastIterationAcrossActive();

    ComponentsPerPosMap &getComponents_FastLookupByChunkPos();

private:
    void processPendingUpdates(BlockSource *region);

    void processPendingAdds();

    void processPendingRemoves();

    void scheduleRelationshipUpdate(const BlockPos &pos, BaseCircuitComponent *component);

    void findRelationships(const BlockPos &pos, BaseCircuitComponent *producerTarget, BlockSource *region);

    void removeStaleRelationships();

    void removeComponent(const BlockPos &pos);

public:
    BaseCircuitComponent *addIfPoweredBlockAt(BlockSource &source, const BlockPos &pos);

    void invalidatePos(const BlockPos &pos);

    void preSetupPoweredBlocks(const ChunkPos &chunkPos);

    void addPositionToReEvaluate(const ChunkPos &chunkPos, const BlockPos &pos);

private:
    ComponentMap         mAllComponents{};
    CircuitComponentList mActiveComponents{};
    ComponentsPerPosMap  mActiveComponentsPerChunk{};
    ComponentsPerPosMap  mPowerAssociationMap{};

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

    std::unordered_map<BlockPos, CircuitSceneGraph::PendingEntry> mPendingAdds{};
    std::unordered_map<BlockPos, CircuitSceneGraph::PendingEntry> mPendingUpdates{};
    std::unordered_map<BlockPos, std::vector<BlockPos>>           mComponentsToReEvaluate{};
    std::vector<CircuitSceneGraph::PendingEntry>                  mPendingRemoves{};
};