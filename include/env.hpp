#pragma once

const double MARS_GRAVITY = 3.71;

struct LanderState {
  double altitude = 500.0;
  double velocity = 0.0;
  double fuel = 2000.0;
  double thrust = 0.0;
  const double mass = 500.0;
  const double weight = mass * MARS_GRAVITY;
  const double drag_coeff = 0.8;
  const double area_cross_section = 5.0;
};

class MarsLanderEnv {
private:
    LanderState state;
    const double MARS_G = -MARS_GRAVITY;
    const double FUEL_BURN_RATE = 0.005;
    const double MARS_AIR_DENSITY = 0.02;

public:
    MarsLanderEnv() = default;
    const double MAX_THRUST = 12000.0;
    const double dt = 0.1;
    
    void reset();
    LanderState get_state() const;
    bool is_terminal() const;
    void step(double thrust_action);
    double calculate_reward() const;
};
