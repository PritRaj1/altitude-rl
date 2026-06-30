#include "env.hpp"

#include <algorithm>
#include <cmath>

using namespace std;

void MarsLanderEnv::reset() {
  state.altitude = 100.0;
  state.velocity = 0.0;
  state.fuel = 2000.0;
  state.thrust = 0.0;
}

LanderState MarsLanderEnv::get_state() const { return state; }

bool MarsLanderEnv::is_terminal() const { return state.altitude <= 0.0; }

void MarsLanderEnv::step(double thrust) {
  double total_mass = DRY_MASS + state.fuel;
  state.thrust = max(0.0, min(thrust, MAX_THRUST));

  double fuel_needed = state.thrust * FUEL_BURN_RATE * dt;
  if (fuel_needed > state.fuel) {
    fuel_needed = state.fuel;
    state.thrust = fuel_needed / (FUEL_BURN_RATE * dt);
  }
  state.fuel -= fuel_needed;

  double drag_magnitude = 0.5 * MARS_AIR_DENSITY *
                          (state.velocity * state.velocity) * DRAG_COEFF *
                          AREA_CROSS_SECTION;
  double drag_force = (state.velocity > 0.0) ? -drag_magnitude : drag_magnitude;
  double acceleration = ((state.thrust + drag_force) / total_mass) + MARS_G;

  state.velocity += acceleration * dt;
  state.altitude += state.velocity * dt;

  if (state.altitude < 0.0) {
    state.altitude = 0.0;
  }
}

double MarsLanderEnv::calculate_reward(const LanderState &prev_state,
                                       double thrust) const {

  double reward = K_alt * (prev_state.altitude - state.altitude);

  double v = fabs(state.velocity);
  reward += K_vel * (fabs(prev_state.velocity) - v);
  reward -= K_fuel * thrust;

  if (is_terminal()) {
    reward += 5000.0 * exp(-0.1 * v * v);
  }

  return reward;
}
