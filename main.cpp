#include <iostream>
#include "redstone/circuit/CircuitSystem.h"
#include "redstone/circuit/components/ProducerComponent.h"
#include "redstone/circuit/components/RepeaterCapacitor.h"
#include "redstone/circuit/components/TransporterComponent.h"
#include "redstone/circuit/components/DirectionalProducerComponent.h"

int main(int, char **)
{
    BlockSource   region;
    CircuitSystem circuitSystem{};
    circuitSystem.lockGraph(false);

    circuitSystem.updateDependencies(&region);
    circuitSystem.evaluate(&region);

    ProducerComponent    *producer = circuitSystem.create<ProducerComponent>({0, 0, 0}, &region, CircuitComponentType::ProducerComponent, Facing::DOWN);
    TransporterComponent *rsWire = circuitSystem.create<TransporterComponent>({1, 0, 0}, &region, CircuitComponentType::TransporterComponent, Facing::DOWN);
    TransporterComponent *rsWire2 = circuitSystem.create<TransporterComponent>({2, 0, 0}, &region, CircuitComponentType::TransporterComponent, Facing::DOWN);
    RepeaterCapacitor    *repeater = circuitSystem.create<RepeaterCapacitor>({3, 0, 0}, &region, CircuitComponentType::RepeaterCapacitor, Facing::WEST);

    circuitSystem.updateDependencies(&region);
    circuitSystem.evaluate(&region);

    std::cout << "producer at [0, 0, 0], strength: " << producer->getStrength() << '\n';
    std::cout << "repeater at [3, 0, 0], strength: " << repeater->getStrength() << '\n';

    producer->setStrength(2);

    circuitSystem.updateDependencies(&region);
    circuitSystem.evaluate(&region);

    std::cout << "producer at [0, 0, 0], strength: " << producer->getStrength() << '\n';
    std::cout << "repeater at [3, 0, 0], strength: " << repeater->getStrength() << '\n';

    producer->setStrength(1);

    circuitSystem.updateDependencies(&region);
    circuitSystem.evaluate(&region);

    std::cout << "producer at [0, 0, 0], strength: " << producer->getStrength() << '\n';
    std::cout << "repeater at [3, 0, 0], strength: " << repeater->getStrength() << '\n';

    return 0;
}
