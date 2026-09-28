#pragma once
// Implement Controller so that, given only the target angle, the last
// measured angle, and the timestep, it drives the system to the target --
// despite whatever nonlinearity you identified from the CSVs.
//
// This is the file you submit. You can add private members, helper methods,
// filters, whatever your design needs. We will never run your internals.

#include "controller_interface.hpp"

class Controller : public IController {
public:
  double update(double target, double measured, double dt) override {
    double error = target - measured;
    // since integral keeps accumulating, we need to store it.
    integral += error * dt;
    // raw derivative (without filter) is just change in measured / dt, so we
    // just store prev_measured
    double raw_derivative = -(measured - prev_measured) / dt;
    prev_measured = measured;
    double alpha = dt / (RC + dt);
    dFiltered = raw_derivative * alpha + (1 - alpha) * dFiltered;
    return kp * error + ki * integral + kd * dFiltered;
  }

  void reset() override {
    integral = 0.0;
    prev_measured = 0.0;
    dFiltered = 0.0;
  }

private:
  // coefficients
  double kp = 3.4;
  double ki = 0.0;
  double kd = 0.110;

  // state
  double integral = 0.0;
  double prev_measured = 0.0;
  double RC = 0.05;       // filter time constant
  double dFiltered = 0.0; // filter applied to the derivative
};
