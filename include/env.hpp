#pragma once

struct LanderState {
  double altitude = 100.0;
  double velocity = 0.0;
  double fuel = 1000.0;
  double thrust = 0.0;
};

class MarsLanderEnv {
private:
  LanderState state;
  const double FUEL_BURN_RATE = 0.001;
  const double MARS_AIR_DENSITY = 0.02;
  const double DRAG_COEFF = 0.8;
  const double AREA_CROSS_SECTION = 5.0;

public:
  MarsLanderEnv() = default;
  const double MARS_G = -3.71;
  const double DRY_MASS = 200.0;
  const double MAX_THRUST = 12000.0;
  const double dt = 0.1;

  void reset();
  LanderState get_state() const;
  bool is_terminal() const;
  void step(double thrust);
  double calculate_reward() const;
};
