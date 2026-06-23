#include "control.hpp"
#include "env.hpp"
#include "utils.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

using namespace std;

int main() {
  MarsLanderEnv env;
  CascadedController controller(-2.0, 1600.0, 100.0, 200.0, 0.25, env);

  while (!env.is_terminal()) {
    LanderState current_state = env.get_state();
    double thrust = controller.action(current_state);
    env.step(thrust);
    LanderState next_state = env.get_state();

    cout << "\033[2J\033[1;1H";
    draw_lander(next_state.altitude, thrust, env.MAX_THRUST);

    this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  LanderState state = env.get_state();
  if (abs(state.velocity) < 5.0) {
    cout << "Safe landing at: " << state.velocity << " m/s\n";
  } else {
    cout << "Crash landed at: " << state.velocity << " m/s\n";
  }

  return 0;
}
