#include "Tick.h"

#include <limits>

const Tick Tick::MAX = std::numeric_limits<uint64_t>::max();

const Tick &Tick::operator=(const Tick &t)
{
    this->tickID = t.tickID;
    return *this;
}

const Tick &Tick::operator=(const uint64_t &t)
{
    this->tickID = t;
    return *this;
}

uint64_t Tick::getTimeStamp() const
{
    return this->tickID;
}

bool Tick::operator!=(const Tick &other) const
{
    return this->tickID != other.tickID;
}

bool Tick::operator<(const Tick &other) const
{
    return this->tickID < other.tickID;
}

void Tick::operator++()
{
    ++this->tickID;
}

Tick Tick::operator+(int o) const
{
    return Tick(this->tickID + o);
}

bool Tick::operator>(const Tick &other) const
{
    return this->tickID > other.tickID;
}

bool Tick::operator>=(const Tick &other) const
{
    return this->tickID >= other.tickID;
}

bool Tick::operator<=(const Tick &other) const
{
    return this->tickID <= other.tickID;
}

bool Tick::operator==(const Tick &other) const
{
    return this->tickID == other.tickID;
}

uint64_t Tick::operator%(int mod) const
{
    return this->tickID % mod;
}