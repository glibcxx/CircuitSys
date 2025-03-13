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

    void add(BaseCircuitComponent *component, int dampening, const BlockPos &pos) { this->mComponents.push_back({component, dampening, pos}); }

    bool removeSource(const BlockPos &posSource, const BaseCircuitComponent *component)
    {
        for (auto it = this->mComponents.begin(); it != this->mComponents.end(); ++it)
        {
            if (it->mPos == posSource)
            {
                it = this->mComponents.erase(it);
                return true;
            }
        }
        return false;
    }

    std::vector<Item> mComponents;

    std::vector<Item>::iterator begin() { return this->mComponents.begin(); }

    std::vector<Item>::iterator end() { return this->mComponents.end(); }

    void push_back(const Item &item) { this->mComponents.push_back(item); }

    std::vector<Item>::iterator erase(std::vector<Item>::const_iterator iter) { return this->mComponents.erase(iter); }

    std::size_t size() const { return this->mComponents.size(); }
};
