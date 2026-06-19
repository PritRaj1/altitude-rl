#include "env.hpp"
#include "control.hpp"

#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>

using namespace std;

int main() {
    MarsLanderEnv env;
    CascadedController controller(0.0, 0.8, 0.1, 0.3);

    while (!env.is_terminal()) {
        LanderState current_state = env.get_state();
        double thrust = controller.action(current_state);
        env.step(thrust);
        LanderState next_state = env.get_state();

        cout << "Altitude: " << fixed << next_state.altitude << " m\t"
                  << "Velocity: " << next_state.velocity << " m/s\t"
                  << "Fuel: " << next_state.fuel << " kg\n";

        this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    LanderState state = env.get_state();
    if (state.velocity < 5.0) {
        cout << "Safe landing at: " << state.velocity << " m/s\n";
    } else {
        cout << "Crash landed at: " << state.velocity << " m/s\n";
    }

    return 0;
}
