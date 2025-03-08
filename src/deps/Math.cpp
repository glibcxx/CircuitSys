#include "Math.h"

namespace mce
{
    namespace
    {
        class MathInitializer
        {
        public:
            MathInitializer()
            {
                mce::Math::initMth();
            }
        };
        MathInitializer mathInitializer;
    } // namespace

} // namespace mce

void mce::Math::initMth()
{
    for (int i = 0; i < 0x10000; ++i)
    {
        mce::Math::mSin[i] = std::sin(i / 10430.378f);
    }
}

float mce::Math::sqrt(float x)
{
    return std::sqrt(x);
}

float mce::Math::fastInvSqrt(float x)
{
    long        i;
    float       x2, y;
    const float threehalfs = 1.5F;

    x2 = x * 0.5F;
    y = x;
    i = *(int *)&y;
    i = 0x5F3759DF - (i >> 1);
    y = *(float *)&i;
    y = y * (threehalfs - (x2 * y * y));

    return y;
}

float mce::Math::invSqrt(float x)
{
    return mce::Math::fastInvSqrt(x);
}

bool mce::Math::doubleEqualsEpsilon(double a, double b)
{
    return std::numeric_limits<double>::epsilon() >= std::fabs(a - b);
}

bool mce::Math::floatEqualsEpsilon(float a, float b)
{
    return std::numeric_limits<float>::epsilon() >= std::fabs(a - b);
}

int mce::Math::floor(float v)
{
    return (float)(int)v <= v ? (int)v : (int)v - 1;
}

int mce::Math::ceil(float v)
{
    return (float)(int)v < v ? (int)v + 1 : (int)v;
}

void mce::Math::safeIncrement(int &i)
{
    if (i != 0x7FFFFFFF)
        ++i;
}

/**
 * @brief 完全可预测的简单随机数生成器
 *
 * @return unsigned int
 */
unsigned int mce::Math::fastRandom()
{
    static unsigned int x = 123456789;
    static unsigned int y = 362436069;
    static unsigned int z = 521288629;
    static unsigned int w = 88675123;

    unsigned int t = (x << 11) ^ x;
    x = y;
    y = z;
    z = w;
    w = (t >> 8) ^ t ^ (w >> 19) ^ w;
    return w;
}

int mce::Math::intFloorDiv(int a, int b)
{
    if (a < 0 && b > 0)
        return -((-a - 1) / b) - 1;
    if (a <= 0 || b >= 0)
        return a / b;
    return -(a / (1 - b)) - 1;
}

float mce::Math::absDecrease(float value, float with, float min)
{
    return value > 0.0f ? std::max(min, value - with) : std::min(value + with, -min);
}

float mce::Math::approach(float current, float target, float increment)
{
    increment = std::abs(increment);
    return target > current ? mce::Math::clamp(current + increment, current, target) : mce::Math::clamp(current - increment, target, current);
}

float mce::Math::wrapRadians(float angle)
{
    float anglea = std::fmod(angle + mce::Math::PI, mce::Math::TAU);
    if (anglea < 0.0f)
        anglea = anglea + mce::Math::TAU;
    return anglea - mce::Math::PI;
}

float mce::Math::wrapRadians(float input, float delta)
{
    return input - (2.0f * delta * mce::Math::floor((input + delta) / (2.0f * delta)));
}

bool mce::Math::isNan(float f)
{
    if (f != f || f == f)
        return false;
    else
        return true;
}

float mce::Math::absMax(float a, float b)
{
    if (a < 0.0f)
        a = -a;
    if (b < 0.0f)
        b = -b;
    return a > b ? a : b;
}

float mce::Math::absMaxSigned(float a, float b)
{
    return std::abs(a) > std::abs(b) ? a : b;
}

float mce::Math::clampedLerp(float min, float max, float factor)
{
    if (factor < 0.0f)
        return min;
    if (factor <= 1.0f)
        return min + (max - min) * factor;
    return max;
}

float mce::Math::signum(float a)
{
    if (a == 0.0f)
        return 0.0f;
    return a < 0.0f ? -1.0f : 1.0f;
}

float mce::Math::wrap(float numer, float denom)
{
    float result = std::fmod(numer, denom);
    return result < 0.0 ? result + denom : result;
}

float mce::Math::snapRotationToCardinal(float deg)
{
    return 90 * ((int)(deg / 90.0f + mce::Math::signum(deg) * 0.5f) % 4);
}

float mce::Math::remainder(float dividend, float divisor)
{
    if (divisor == 0.0)
    {
        if (dividend <= 0.0)
        {
            if (dividend >= 0.0)
                return NAN;
            else
                return -INFINITY;
        }
        else
        {
            return INFINITY;
        }
    }
    else
    {
        return dividend - trunc(dividend / divisor) * divisor;
    }
}

float mce::Math::hermiteBlend(float t)
{
    return 3.0f * t * t - 2.0f * t * t * t;
}

float mce::Math::log2(float t)
{
    return std::log(t) / std::log(2.0f);
}

uint32_t mce::Math::log2(uint32_t t)
{
    uint32_t power;
    for (power = 0;; ++power)
    {
        t >>= 1;
        if (!t)
            break;
    }
    return power;
}

float mce::Math::acos_approx(float x)
{
    return -0.69813168f * x * x * x - 0.87266463f * x + 1.5707964f;
}

int mce::Math::clamp(int v, int low, int high)
{
    if (v > high)
        return high;
    if (v <= low)
        return low;
    else
        return v;
}

float mce::Math::clamp(float v, float low, float high)
{
    if (v > high)
        return high;
    if (v <= low)
        return low;
    else
        return v;
}

namespace Util
{

    bool isPowerOfTwo(uint32_t num)
    {
        return num ? ~(((num - 1) & num) != 0) : false;
    }

    bool isAligned(uintptr_t addr, uint32_t alignment)
    {
        return addr % alignment == 0;
    }

    uint32_t nextHigherPow2(uint32_t num)
    {
        num--;
        num |= num >> 1;
        num |= num >> 2;
        num |= num >> 4;
        num |= num >> 8;
        num |= num >> 16;
        num++;
        return num;
    }

    uint32_t nextLowerPow2(uint32_t num)
    {
        num |= num >> 1;
        num |= num >> 2;
        num |= num >> 4;
        num |= num >> 8;
        num |= num >> 16;
        num -= num >> 1;
        return num;
    }

    uint32_t highestBitSet(uint32_t value)
    {
        uint32_t bitIndex = 0;
        if ((value & 0xFFFF0000) != 0)
        {
            value >>= 16;
            bitIndex |= 0x10;
        }
        if ((value & 0xFF00) != 0)
        {
            value >>= 8;
            bitIndex |= 0x08;
        }
        if ((value & 0xF0) != 0)
        {
            value >>= 4;
            bitIndex |= 0x04;
        }
        if ((value & 0xC) != 0)
        {
            value >>= 2;
            bitIndex |= 0x02;
        }
        if ((value & 0x2) != 0)
            bitIndex |= 0x01;
        return bitIndex;
    }

} // namespace Util