#pragma once

#include <cstdint>
#include <string>
#include <cmath>
#include <type_traits>

namespace mce
{
    class Math
    {
    private:
        inline static float       mSin[0x10000]{};
        inline static const float mSinScale = 10430.378f;

    public:
        static constexpr float PI = 3.1415927F;
        static constexpr float HALF_PI = 1.5707964F;
        static constexpr float TAU = 6.2831855F;
        static constexpr float DEGRAD = 0.017453292F;
        static constexpr float RADDEG = 57.295776F;
        static constexpr float E = 2.7182817F;

        static void initMth();

        static float sqrt(float x);

        static float fastInvSqrt(float x);

        static float invSqrt(float x);

        static bool doubleEqualsEpsilon(double a, double b);

        static bool floatEqualsEpsilon(float a, float b);

        static int floor(float v);

        static int ceil(float v);

        static float cos(float f);

        static float sin(float f);

        template <typename T>
        static T sign(T input)
        {
            if (input > 0.0f)
                return 1;
            if (input < 0.0f)
                return -1;
            return 0;
        }

        static void safeIncrement(int &i);

        static unsigned int fastRandom();

        static int intFloorDiv(int a, int b);

        static float absDecrease(float value, float with, float min);

        template <typename T1, typename T2>
        static auto max(T1 a, T2 b) -> typename std::remove_reference_t<decltype(a > b ? a : b)>
        {
            return a > b ? a : b;
        }

        template <typename T1, typename T2>
        static auto min(T1 a, T2 b) -> std::remove_reference_t<decltype(a < b ? a : b)>
        {
            return a < b ? a : b;
        }

        template <typename T>
        static T clamp(T v, T low, T high)
        {
            if (v > high)
                return high;
            if (v <= low)
                return low;
            else
                return v;
        }

        static float degreesDifference(float angleA, float angleB);

        static float degreesDifferenceAbs(float angleA, float angleB);

        static float approach(float current, float target, float increment);

        static float approachDegrees(float current, float target, float increment);

        static float wrapRadians(float angle);

        static float wrapDegrees(float input);

        static float wrapRadians(float input, float delta);

        static float clampRotate(float current, float target, float maxDelta);

        template <typename T>
        static T lerp(const T &src, const T &dst, float alpha)
        {
            return (dst - src) * alpha + src;
        }

        static float lerpRotate(float from, float to, float a);

        static bool isNan(float f);

        static float absMax(float a, float b);

        static float absMaxSigned(float a, float b);

        static float clampedLerp(float min, float max, float factor);

        static float signum(float a);

        static float wrap(float numer, float denom);

        static float snapRotationToCardinal(float deg);

        static float remainder(float dividend, float divisor);

        static float hermiteBlend(float t);

        static float log2(float t);

        static uint32_t log2(uint32_t t);

        static float acos_approx(float x);

        static int clamp(int v, int low, int high);

        static float clamp(float v, float low, float high);

        template <typename T>

        static int round(T val)
        {
            return (int)std::round(val);
        }

        template <typename T>

        static size_t hash_accumulate(size_t hash, const T &toHash)
        {
            return ((hash >> 2) + (hash << 6) + std::hash<T>()(toHash) + 2654435769u) ^ hash;
        }

        template <typename T1, typename T2>

        static size_t hash2(const T1 &a, const T2 &b)
        {
            size_t hash = mce::Math::hash_accumulate<T1>(0LL, a);
            return mce::Math::hash_accumulate<T2>(hash, b);
        }

        template <typename T1, typename T2, typename T3>

        static size_t hash3(const T1 &a, const T2 &b, const T3 &c)
        {
            size_t hash = mce::Math::hash_accumulate<T2>(mce::Math::hash_accumulate<T1>(0LL, a), b);
            return mce::Math::hash_accumulate<T3>(hash, c);
        }

        template <typename T>

        static auto square(const T &n) -> typename std::remove_reference<decltype(n * n)>::type
        {
            return n * n;
        }
    };

} // namespace mce

template <typename T>
class buffer_span;

namespace Util
{
    template <typename T>
    T swapEndian(T u)
    {
        union
        {
            T       u;
            uint8_t u8[sizeof(T)];
        } source, dest;
        source.u = u;
        for (size_t k = 0; k < sizeof(T); ++k)
            dest.u8[k] = source.u8[sizeof(T) - k - 1];
        return dest.u;
    }

    template <typename T>
    bool isAllZero(buffer_span<T> span)
    {
        size_t   accum;
        const T *ptr = span.data();
        const T *end = &ptr[2 * (span.byte_size() >> 3)];
        for (accum = 0; ptr < end && accum == 0; ptr += 2)
        {
            accum |= *(size_t *)&ptr;
        }
        return accum == 0;
    }

    inline void memcpy_strided(void *dst, size_t dstStride, const void *src, size_t srcStride, size_t elementSize, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            memcpy(dst, src, elementSize);
            dst = (char *)dst + dstStride;
            src = (char *)src + srcStride;
        }
    }

}; // namespace Util
