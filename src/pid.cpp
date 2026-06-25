#include "control.hpp"

#include <algorithm>
#include <cmath>

using namespace std;

CascadedController::CascadedController(double target, double p_gain,
                                       double i_gain, double d_gain,
                                       double p_gain_alt,
                                       const MarsLanderEnv &env)
    : target(target), Kp_vel(p_gain), Ki_vel(i_gain), Kd_vel(d_gain),
      Kp_alt(p_gain_alt), env(env), prev_err(0.0), acc_err(0.0),
      first_run(true) {}

// Inner PID loop to zero velocity error
double CascadedController::pid(double err) {
  double P = Kp_vel * err; // proportional

  acc_err += err * env.dt;
  acc_err = max(-100.0, min(acc_err, 100.0)); // anti-windup
  double I = Ki_vel * acc_err;                // integral

  // derivative
  double D = 0.0;
  if (!first_run) {
    double derivative = (err - prev_err) / env.dt;
    D = Kd_vel * derivative;
  } else {
    first_run = false;
  }

  prev_err = err;
  return P + I + D;
}

double CascadedController::action(const LanderState &state) {

  // Outer loop: target velocity
  double command_velocity = -Kp_alt * state.altitude;
  double err = command_velocity - state.velocity;
  double pid_force = pid(err);

  double mass = env.DRY_MASS + state.fuel;
  double total_thrust = mass * (env.MARS_G + pid_force);
  total_thrust = clamp(total_thrust, 0.0, env.MAX_THRUST);

  if (state.fuel <= 0.0) {
    total_thrust = 0.0;
  }

  return total_thrust;
}
