#pragma once

#define CS_BaseCircuit 1129534275  // 基类元件
#define CS_PoweredBlock 1129533506 // 充能方块
#define CS_Producer 1129533507     // 生产者
#define CS_Capacitor 1129530177    // 电容器
#define CS_Transporter 1129534546  // 传输者
#define CS_Consumer 1129530179     // 消费者
#define CS_Comparator 1296253778   // 比较器
#define CS_Pulse 1296257091        // 观察者
#define CS_Piston 1296257097       // 活塞
#define CS_BaseRail 1296257106     // 铁轨
#define CS_Repeater 1296257618     // 中继器
#define CS_Torch 1296258115        // 红石火把

namespace Redstone
{
    inline const int SIGNAL_NONE = 0;
    inline const int SIGNAL_MIN = 0;
    inline const int SIGNAL_MAX = 15;
    inline const int SIGNAL_LEVELS = 16;

}; // namespace Redstone
