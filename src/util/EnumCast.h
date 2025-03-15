#pragma once

#include <type_traits>

template <typename T, std::enable_if_t<!std::is_enum_v<T>, std::nullptr_t> = nullptr>
decltype(auto) EnumCastHelper(T integral)
{
    return integral;
}

template <typename T, std::enable_if_t<std::is_enum_v<T>, std::nullptr_t> = nullptr>
decltype(auto) EnumCastHelper(T enumValue)
{
    return static_cast<std::underlying_type_t<T>>(enumValue);
}

template <typename T>
decltype(auto) enum_cast(T t)
{
    return EnumCastHelper<T>(t);
}