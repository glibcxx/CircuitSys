#pragma once

#include <vector>
#include <memory>
#include "gsl/gsl"
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

    void updateIndividualBlock(gsl::not_null<BaseCircuitComponent *> component, const BlockPos &chunkPos, const BlockPos &pos, BlockSource &region);

public:
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
    ComponentClass *getComponent(const BlockPos &pos, CircuitComponentType type)
    {
        ComponentClass *component = static_cast<ComponentClass *>(this->mSceneGraph.getComponent(pos, type));
        if (component)
            return component;
        else
            return static_cast<ComponentClass *>(this->mSceneGraph.getFromPendingAdd(pos, type));
    }

    bool isAvailableAt(const BlockPos &pos);

    bool isConnectionAllowed(BaseCircuitComponent &component, const BlockPos &pos, FacingID face, bool bDirectlyPowered);

public:
    void invalidatePos(const BlockPos &pos);

    void preSetupPoweredBlocks(const ChunkPos &chunkPos);

    void lockGraph(bool bLock);

private:
    bool              mLockGraph = false;
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
