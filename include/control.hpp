#pragma once
#include "env.hpp"

class ThrustController {
public:
    virtual ~ThrustController() = default;
    virtual double action(const LanderState& state) = 0;
};

class CascadedController : public ThrustController {
private:
    double target;
    double dt;

    // Gains
    double Kp_vel;
    double Ki_vel;
    double Kd_vel;
    double Kp_alt;
    
    // Inner Loop PID: corrects velocity
    double acc_err;
    bool first_run;
    double pid(double err);

public:
    double prev_err;
    CascadedController(double target, double p_gain, double i_gain, double d_gain, double p_gain_alt, double dt);
    double action(const LanderState& state) override;
};
