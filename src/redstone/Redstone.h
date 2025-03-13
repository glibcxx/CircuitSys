#pragma once

#include <cstdint>

enum class CircuitComponentType : int64_t
{
    Unknown = 0,
    Undefined = 1,
    Mask = 0xFFFF0000,
    BaseCircuitComponent = 1ull << 31,  // 基类元件
    BaseRailTransporter = 1ull << 16,   // 铁轨
    ConsumerComponent = 1ull << 17,     // 消费者
    PoweredBlockComponent = 1ull << 18, // 充能方块
    ProducerComponent = 1ull << 19,     // 生产者
    TransporterComponent = 1ull << 20,  // 传输者
    CapacitorComponent = 1ull << 21,    // 电容器

    PistonConsumer = 1 | ConsumerComponent, // 活塞

    ComparatorCapacitor = 1 | CapacitorComponent,    // 比较器
    PulseCapacitor = 2 | CapacitorComponent,         // 观察者
    RedstoneTorchCapacitor = 3 | CapacitorComponent, // 红石火把
    RepeaterCapacitor = 4 | CapacitorComponent,      // 中继器
};

namespace Redstone
{
    constexpr int SIGNAL_NONE = 0;
    constexpr int SIGNAL_MIN = 0;
    constexpr int SIGNAL_MAX = 15;
    constexpr int SIGNAL_LEVELS = 16;

}; // namespace Redstone
