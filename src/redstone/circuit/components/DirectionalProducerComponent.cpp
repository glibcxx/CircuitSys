#include "DirectionalProducerComponent.h"

bool DirectionalProducerComponent::allowConnection(CircuitSceneGraph &graph, const CircuitTrackingInfo &info, bool &bDirectlyPowered)
{
    FacingID dir = info.mCurrent.mDirection;
    if (dir != Facing::NOT_DEFINED && !this->mAllowedConnections.test(dir))
    {
        return false;
    }
    return this->ProducerComponent::allowConnection(graph, info, bDirectlyPowered);
}

void DirectionalProducerComponent::setAllowedConnections(std::bitset<6> connections)
{
    this->mAllowedConnections = connections;
}
