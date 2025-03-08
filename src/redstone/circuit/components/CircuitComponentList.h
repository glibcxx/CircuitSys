#pragma once

#include <cstdint>
#include <vector>
#include "level/BlockPos.h"

class BaseCircuitComponent;

class CircuitComponentList
{
public:
    struct Item
    {
        BaseCircuitComponent *mComponent = nullptr;
        int                   mDampening = 0;
        BlockPos              mPos{};
        FacingID              mDirection = Facing::NOT_DEFINED;
        bool                  mDirectlyPowered = false;
        int                   mData = 0;

        Item() = default;

        Item(BaseCircuitComponent *component, int dampening, const BlockPos &pos) :
            mComponent(component),
            mDampening(dampening),
            mPos(pos) {}
    };

    void add(BaseCircuitComponent *component, int dampening, const BlockPos &pos);

    bool removeSource(const BlockPos &posSource, const BaseCircuitComponent *component);

    std::vector<Item> mComponents;

    std::vector<Item>::iterator begin();

    std::vector<Item>::iterator end();

    void push_back(const Item &item);

    std::vector<Item>::iterator erase(std::vector<Item>::const_iterator iter);

    int size() const;
};
