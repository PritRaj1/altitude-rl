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

    // Gains
    double Kp_vel;
    double Ki_vel;
    double Kd_vel;
    
    // Inner Loop PID: corrects velocity
    double prev_err;
    double acc_err;
    bool first_run;
    double pid(double err, double dt);

public:
    CascadedController(double target, double p_gain, double i_gain, double d_gain);
    double action(const LanderState& state) override;
};
