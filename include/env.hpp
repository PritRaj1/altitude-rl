#pragma once

struct LanderState {
  double altitude = 100.0;
  double velocity = -50.0;
  double mass = 800.0;
  double fuel = 300.0;
  double thrust = 0.0;
  double weight = 800.0;
};

class MarsLanderEnv {
private:
    LanderState state;
    const double MARS_G = -3.71;
    const double MAX_THRUST = 12000.0;
    const double FUEL_BURN_RATE = 0.05;
    const double dt = 0.1;

public:
    MarsLanderEnv() = default;
    
    void reset();
    LanderState get_state() const;
    bool is_terminal() const;
    void step(double thrust_action);
    double calculate_reward() const;
};
