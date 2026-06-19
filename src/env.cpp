#include "env.hpp"
#include <algorithm>
#include <cmath>

using namespace std;

void MarsLanderEnv::reset() {
    state = LanderState();
}

LanderState MarsLanderEnv::get_state() const { 
    return state; 
}

bool MarsLanderEnv::is_terminal() const { 
    return state.altitude <= 0.0; 
}

void MarsLanderEnv::step(double thrust_action) {
    state.thrust = max(0.0, min(thrust_action, MAX_THRUST));

    double fuel_needed = state.thrust * FUEL_BURN_RATE * dt;
    if (fuel_needed > state.fuel) {
        fuel_needed = state.fuel;
        state.thrust = fuel_needed / (FUEL_BURN_RATE * dt);
    }
    state.fuel -= fuel_needed;

    double total_mass = state.mass + state.fuel;
    double acceleration = (state.thrust / total_mass) + MARS_G;

    state.velocity += acceleration * dt;
    state.altitude += state.velocity * dt;

    if (state.altitude < 0.0) {
        state.altitude = 0.0;
    }
}
