#pragma once

#include <cstdint>

enum class Mirror : uint8_t
{
    None = 0,
    X = 1,
    Z = 2,
    XZ = 3,
};
