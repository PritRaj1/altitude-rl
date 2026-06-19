#include "env.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>

using namespace std;

int main() {
    MarsLanderEnv env;

    while (!env.is_terminal()) {
        LanderState current_state = env.get_state();
        env.step(0.0);
        LanderState next_state = env.get_state();

        std::cout << "Altitude: " << std::fixed << next_state.altitude << " m\t"
                  << "Velocity: " << next_state.velocity << " m/s\t"
                  << "Fuel: " << next_state.fuel << " kg\n";

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
