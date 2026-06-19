#include "control.hpp"

#include <cmath>
#include <algorithm>

using namespace std;

CascadedController::CascadedController(double target, double p_gain, double i_gain, double d_gain) 
    : target(target), Kp_vel(p_gain), Ki_vel(i_gain), Kd_vel(d_gain), 
      prev_err(0.0), acc_err(0.0), first_run(true) {}

// Inner PID loop to zero velocity error
double CascadedController::pid(double err, double dt) {
    double P = Kp_vel * err; // proportional
    
    acc_err += err * dt;
    acc_err = max(-50.0, min(acc_err, 50.0)); // anti-windup
    double I = Ki_vel * acc_err; // integral

    // derivative
    double D = 0.0;
    if (!first_run) {
        double derivative = (err - prev_err) / dt;
        D = Kd_vel * derivative;
    } else {
        first_run = false;
    }
    
    prev_err = err;
    return P + I + D;
}

double CascadedController::action(const LanderState& state) {
    const double dt = 0.1;

    // Outer guidance loop (altitude -> velocity)
    double err = target - state.velocity;
    double pid_force = pid(err, dt);
    double total_thrust = state.weight + pid_force;
    return total_thrust;
}
