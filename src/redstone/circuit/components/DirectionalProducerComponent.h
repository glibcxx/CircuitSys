#pragma once

#include <bitset>
#include "ProducerComponent.h"

class DirectionalProducerComponent : public ::ProducerComponent
{
public:
    virtual ~DirectionalProducerComponent() {}

    virtual bool allowConnection(class CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered);

    DirectionalProducerComponent() {}

    void setAllowedConnections(std::bitset<6> connections);

    std::bitset<6> mAllowedConnections{};
};
