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
  acc_err = max(-50.0, min(acc_err, 50.0)); // anti-windup
  double I = Ki_vel * acc_err;              // integral

  // derivative
  double D = 0.0;
  if (!first_run) {
    double derivative = (err - prev_err) / env.dt;
    D = -Kd_vel * derivative;
  } else {
    first_run = false;
  }

  prev_err = err;
  return P + I + D;
}

double CascadedController::action(const LanderState &state) {

  // Outer loop: energy-based target velocity
  double command_velocity = -sqrt(max(0.0, 2.0 * env.MARS_G * state.altitude));
  command_velocity = ::max(command_velocity, -20.0);

  double err = command_velocity - state.velocity;
  double pid_force = pid(err);

  double mass = env.DRY_MASS + state.fuel;
  double gravity_force = mass * env.MARS_G;

  double total_thrust = gravity_force + pid_force;
  total_thrust = clamp(total_thrust, 0.0, env.MAX_THRUST);

  if (state.fuel <= 0.0) {
    total_thrust = 0.0;
  }

  return total_thrust;
}
