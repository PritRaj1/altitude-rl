#pragma once

struct LanderState {
  double altitude = 100.0;
  double velocity = 0.0;
  double fuel = 3000.0;
  double thrust = 0.0;
  double weight = 200.0;
  const double mass = 500.0;
  const double drag_coeff = 0.8;
  const double area_cross_section = 5.0;
};

class MarsLanderEnv {
private:
    LanderState state;
    const double MARS_G = -3.71;
    const double MAX_THRUST = 12000.0;
    const double FUEL_BURN_RATE = 0.003;
    const double MARS_AIR_DENSITY = 0.02;
    const double dt = 0.1;

public:
    MarsLanderEnv() = default;
    
    void reset();
    LanderState get_state() const;
    bool is_terminal() const;
    void step(double thrust_action);
    double calculate_reward() const;
};
