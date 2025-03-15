#include <iostream>
#include "level/block/Block.h"
#include "redstone/circuit/CircuitSystem.h"
#include "redstone/circuit/components/ProducerComponent.h"
#include "redstone/circuit/components/RepeaterCapacitor.h"
#include "redstone/circuit/components/TransporterComponent.h"
#include "redstone/circuit/components/DirectionalProducerComponent.h"

int main(int, char **)
{
    FakeBlocks::registryBlocks();

    BlockSource   region;
    CircuitSystem circuitSystem{};
    circuitSystem.lockGraph(false);

    circuitSystem.updateDependencies(&region);
    circuitSystem.evaluate(&region);

    ProducerComponent *producer = circuitSystem.create<ProducerComponent>({0, 0, 0}, &region, CircuitComponentType::ProducerComponent, Facing::DOWN);

    region.setBlock({1, -1, 0}, *FakeBlocks::mQuartzBlock);
    TransporterComponent *rsWire = circuitSystem.create<TransporterComponent>({1, 0, 0}, &region, CircuitComponentType::TransporterComponent, Facing::DOWN);

    region.setBlock({2, -1, 0}, *FakeBlocks::mTopSlabBlock);
    TransporterComponent *rsWire2 = circuitSystem.create<TransporterComponent>({2, 0, 0}, &region, CircuitComponentType::TransporterComponent, Facing::DOWN);

    region.setBlock({3, 0, 0}, *FakeBlocks::mTopSlabBlock);
    TransporterComponent *rsWire3 = circuitSystem.create<TransporterComponent>({3, -1, 0}, &region, CircuitComponentType::TransporterComponent, Facing::DOWN);

    TransporterComponent *rsWire4 = circuitSystem.create<TransporterComponent>({3, 1, 0}, &region, CircuitComponentType::TransporterComponent, Facing::DOWN);

    RepeaterCapacitor *repeater = circuitSystem.create<RepeaterCapacitor>({1, -1, 1}, &region, CircuitComponentType::RepeaterCapacitor, Facing::NORTH);

    circuitSystem.updateDependencies(&region);
    circuitSystem.evaluate(&region);

    std::cout << "producer at [0,  0, 0], strength: " << producer->getStrength() << '\n';
    std::cout << "wire at     [1,  0, 0], strength: " << rsWire->getStrength() << '\n';
    std::cout << "wire at     [2,  0, 0], strength: " << rsWire2->getStrength() << '\n';
    std::cout << "wire at     [3, -1, 0], strength: " << rsWire3->getStrength() << '\n';
    std::cout << "wire at     [3,  1, 0], strength: " << rsWire4->getStrength() << '\n';
    std::cout << "repeater at [1, -1, 1], strength: " << repeater->getStrength() << "\n\n";

    producer->setStrength(15);

    circuitSystem.updateDependencies(&region);
    circuitSystem.evaluate(&region);

    std::cout << "producer at [0,  0, 0], strength: " << producer->getStrength() << '\n';
    std::cout << "wire at     [1,  0, 0], strength: " << rsWire->getStrength() << '\n';
    std::cout << "wire at     [2,  0, 0], strength: " << rsWire2->getStrength() << '\n';
    std::cout << "wire at     [3, -1, 0], strength: " << rsWire3->getStrength() << '\n';
    std::cout << "wire at     [3,  1, 0], strength: " << rsWire4->getStrength() << '\n';
    std::cout << "repeater at [1, -1, 1], strength: " << repeater->getStrength() << "\n\n";

    producer->setStrength(2);

    circuitSystem.updateDependencies(&region);
    circuitSystem.evaluate(&region);

    std::cout << "producer at [0,  0, 0], strength: " << producer->getStrength() << '\n';
    std::cout << "wire at     [1,  0, 0], strength: " << rsWire->getStrength() << '\n';
    std::cout << "wire at     [2,  0, 0], strength: " << rsWire2->getStrength() << '\n';
    std::cout << "wire at     [3, -1, 0], strength: " << rsWire3->getStrength() << '\n';
    std::cout << "wire at     [3,  1, 0], strength: " << rsWire4->getStrength() << '\n';
    std::cout << "repeater at [1, -1, 1], strength: " << repeater->getStrength() << "\n\n";

    FakeBlocks::unregistryBlocks();
    return 0;
}
