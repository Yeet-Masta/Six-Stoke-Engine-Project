#include "six-stroke-engine.h"
#include <iostream>
#include <vector>
#include <random>

int main() {
    SixStrokeEngine engine;

    std::cout << "Advanced Six-Stroke Engine Simulation with Gearbox\n";
    std::cout << "=================================================\n";

    std::vector<std::string> available_upgrades = {
        "direct_injection", "turbocharger", "variable_valve_timing",
        "exhaust_gas_recirculation", "waste_heat_recovery", "smart_cooling",
        "advanced_materials", "enhanced_ecu", "cylinder_deactivation",
        "variable_compression", "ceramic_coating"
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    for (const auto& upgrade : available_upgrades) {
        if (dis(gen)) {
            engine.apply_upgrade(upgrade);
        }
    }

    engine.run_simulation();

    return 0;
}