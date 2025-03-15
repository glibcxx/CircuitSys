#pragma once

#include <cstdint>

struct Tick
{
    static const Tick MAX;

    Tick(uint64_t v) :
        tickID(v) {}

    Tick() {}

    Tick(const Tick &t) :
        tickID(t.tickID) {}

    const Tick &operator=(const Tick &t);
    const Tick &operator=(const uint64_t &t);

    void operator++();
    Tick operator+(int o) const;

    uint64_t operator%(int mod) const;

    bool operator<(const Tick &other) const;
    bool operator>(const Tick &other) const;
    bool operator<=(const Tick &other) const;
    bool operator>=(const Tick &other) const;
    bool operator==(const Tick &other) const;
    bool operator!=(const Tick &other) const;

    uint64_t getTimeStamp() const;

    uint64_t tickID = 0;
};
