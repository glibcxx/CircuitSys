// dwarf does not contain this file

#include "CircuitComponentList.h"

std::vector<CircuitComponentList::Item>::iterator CircuitComponentList::end()
{
    return this->mComponents.end();
}

std::vector<CircuitComponentList::Item>::iterator CircuitComponentList::begin()
{
    return this->mComponents.begin();
}

void CircuitComponentList::push_back(const Item &item)
{
    this->mComponents.push_back(item);
}

int CircuitComponentList::size() const
{
    return this->mComponents.size();
}

/**
 * 从信号源表中移除特定位置的信号源,O(n)
 * @param posSource 位置
 * @param component 无效参数
 * @return 是否成功移除
 */
bool CircuitComponentList::removeSource(const BlockPos &posSource, const BaseCircuitComponent *component)
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

// 往信号源表尾部添加一个新的信号源
void CircuitComponentList::add(BaseCircuitComponent *component, int dampening, const BlockPos &pos)
{
    Item item(component, dampening, pos);
    this->mComponents.push_back(item);
}

std::vector<CircuitComponentList::Item>::iterator CircuitComponentList::erase(std::vector<CircuitComponentList::Item>::const_iterator iter)
{
    return this->mComponents.erase(iter);
}
