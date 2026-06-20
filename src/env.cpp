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

LanderState MarsLanderEnv::get_state() const { 
    return state; 
}

bool MarsLanderEnv::is_terminal() const { 
    return state.altitude <= 0.0; 
}

void MarsLanderEnv::step(double thrust) {
    double total_mass = DRY_MASS + state.fuel;
    state.thrust = max(0.0, min(thrust, MAX_THRUST));

    double fuel_needed = state.thrust * FUEL_BURN_RATE * dt;
    if (fuel_needed > state.fuel) {
        fuel_needed = state.fuel;
        state.thrust = fuel_needed / (FUEL_BURN_RATE * dt);
    }
    state.fuel -= fuel_needed;

    double drag_magnitude = 0.5 * MARS_AIR_DENSITY * (state.velocity * state.velocity) * DRAG_COEFF * AREA_CROSS_SECTION;
    double drag_force = (state.velocity > 0.0) ? -drag_magnitude : drag_magnitude;
    double acceleration = ((state.thrust + drag_force) / total_mass) + MARS_G;

    state.velocity += acceleration * dt;
    state.altitude += state.velocity * dt;

    if (state.altitude < 0.0) {
        state.altitude = 0.0;
    }
}

double MarsLanderEnv::calculate_reward(double thrust) const {
    double reward = -0.1; 
    double fuel_penalty = -0.05 * (thrust / MAX_THRUST);
    reward += fuel_penalty;

    if (state.altitude > 0.0) {
        double target_velocity = -2.0 - (state.altitude * 0.05); 
        double velocity_error = abs(state.velocity - target_velocity);
        reward -= velocity_error * 0.1; 
    }

    if (is_terminal() && state.altitude <= 0.0) {
        if (state.velocity >= -5.0 && state.velocity <= 0.1) {
            reward += 2000.0 + (state.fuel * 0.5); 
        } else {
            reward -= 2000.0 + (abs(state.velocity) * 20.0);
        }
    }
    return reward;
}
