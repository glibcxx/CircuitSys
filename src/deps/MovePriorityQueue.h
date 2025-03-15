#pragma once

#include <vector>
#include <algorithm>
#include "level/BlockPos.h"
#include "level/Tick.h"

class Block;

template <typename T, typename COMP>
class MovePriorityQueue
{
public:
    typedef typename std::vector<T> Base;

    typedef typename std::vector<T>::const_iterator const_iterator;

public:
    template <typename... Args>
    void emplace(Args &&...args)
    {
        this->mC.emplace_back(std::forward<Args>(args)...);
        std::push_heap(this->mC.begin(), this->mC.end(), COMP());
    }

    T pop()
    {
        std::pop_heap(this->mC.begin(), this->mC.end(), COMP());
        T retstr = this->mC.back();
        this->mC.pop_back();
        return retstr;
    }

    const T &top() const
    {
        return this->mC.front();
    }

    bool empty() const
    {
        return this->mC.empty();
    }

    void clear()
    {
        return this->mC.clear();
    }

    void resort()
    {
        if (this->size())
        {
            std::make_heap(this->mC.begin(), this->mC.end(), COMP());
        }
    }

    size_t size() const
    {
        return this->mC.size();
    }

    const_iterator begin() const
    {
        return this->mC.begin();
    }

    const_iterator end() const
    {
        return this->mC.end();
    }

    Base &getInternalContainer()
    {
        return this->mC;
    }

protected:
    Base mC;
};