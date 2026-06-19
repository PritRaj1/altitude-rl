#pragma once
#include "Environment.hpp"

class ThrustController {
public:
    virtual ~ThrustController() = default;
    virtual double action(const LanderState& state) = 0;
};

class PIDController : public ThrustController {
private:
    double Kp;
    double Kd;

public:
    PIDController(double p_gain, double d_gain);
    double action(const LanderState& state) override;
};
