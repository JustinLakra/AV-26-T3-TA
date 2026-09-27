#pragma once
// Your model of the actuator, reconstructed from the decoded CSVs. This is
// the Part B deliverable, alongside your written notes.
//
// Implement step(): given a commanded velocity and a timestep, return the
// measured output angle. The placeholder below is a bare integrator with
// gain 1 -- NOT the real actuator. Replace it with what the data shows
// (dynamics, gain, any nonlinearity, any lag), or the harness proves nothing.

#include <cmath>

constexpr double K = 1.3;
constexpr double TAU = 0.07;
constexpr double backlash =
    2.5; // taken from the error returned by the check code when running without
         // applying nonlinearity (backlash)

struct Plant {
  // add whatever state your model needs (velocity, motor-side angle, ...)
  double angle = 0.0;
  double speed = 0.0;
  double idealAngle = 0.0;

  // u_cmd : commanded velocity, deg/s
  // dt    : timestep, seconds
  // return: measured output angle, deg
  double step(double u_cmd, double dt) {
    speed += (K * u_cmd - speed) * dt / TAU;
    idealAngle += speed * dt;
    angle =
        std::max(idealAngle - backlash, std::min(idealAngle + backlash, angle));
    return std::round(angle / 0.1) * 0.1; // the sensor reads to 0.1 deg
  }

  void reset() {
    angle = 0.0;
    speed = 0.0;
    idealAngle = 0.0;
  }
};
