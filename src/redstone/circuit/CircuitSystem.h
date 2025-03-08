#pragma once

#include <vector>
#include <memory>
#include "CircuitSceneGraph.h"
#include "level/BlockSource.h"

class CircuitSystem
{
public:
    CircuitSystem() {};

    void evaluate(BlockSource *region);

    void updateBlocks(BlockSource &region, const BlockPos &chunkPos);

    void updateDependencies(BlockSource *region);

private:
    void _shouldEvaluate(BlockSource *region);

    void evaluateComponents(bool bOnlyProducers);

    void cacheValues();

    void checkLocks();

    void updateIndividualBlock(BaseCircuitComponent *component, const BlockPos &chunkPos, const BlockPos &pos, BlockSource &region);

public:
    template <typename ComponentClass>
    ComponentClass *create(const BlockPos &pos, BlockSource *pSource, FacingID direction)
    {
        if (this->mLockGraph)
            return this->getComponent<ComponentClass>(pos);
        std::unique_ptr<BaseCircuitComponent> newComponent = std::make_unique<ComponentClass>();
        BaseCircuitComponent                 *Component = this->createComponent(pos, direction, std::move(newComponent));
        if (pSource)
        {
            Component->mChunkPosition = pSource->getPlaceChunkPos();
        }
        return static_cast<ComponentClass *>(Component);
    }

    void removeComponents(const BlockPos &pos);

private:
    BaseCircuitComponent *createComponent(const BlockPos &pos, FacingID direction, std::unique_ptr<BaseCircuitComponent> newComponent);

public:
    static int const NOT_DEFINED_STRENGTH = -1;

    void setStrength(const BlockPos &pos, int strength);

    int getStrength(const BlockPos &pos);

    FacingID getDirection(const BlockPos &pos);

    bool hasDirectPower(const BlockPos &pos);

    template <typename ComponentClass>
    ComponentClass *getComponent(const BlockPos &pos)
    {
        ComponentClass *component = static_cast<ComponentClass *>(this->mSceneGraph.getComponent(pos, ComponentClass::TypeID));
        if (component)
            return component;
        else
            return static_cast<ComponentClass *>(this->mSceneGraph.getFromPendingAdd(pos, ComponentClass::TypeID));
    }

    bool isAvailableAt(const BlockPos &pos);

private:
    static BaseCircuitComponent *getBaseComponent(const BlockPos &pos);

public:
    void invalidatePos(const BlockPos &pos);

    void preSetupPoweredBlocks(const ChunkPos &chunkPos);

    void lockGraph(bool bLock);

private:
    bool              mLockGraph = true;
    CircuitSceneGraph mSceneGraph{};

    const BlockPos getChunkPos(BlockSource *region, const BlockPos &pos);

    class LevelChunkTracking
    {
    public:
        LevelChunkTracking(BlockPos pos);

        BlockPos mChunkPos;
    };

    std::vector<LevelChunkTracking> mAddedLevelChunk;

private:
    bool mHasBeenEvaluated = true;
};
