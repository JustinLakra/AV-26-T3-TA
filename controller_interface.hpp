#pragma once
// Only target, measured, and dt are visible to
// the controller -- it never sees the plant's internal state directly,
// mirroring the real system where only the CAN-reported angle is available.

class IController {
public:
    virtual ~IController() = default;

    // Called once per control tick.
    // target   = desired output angle, degrees
    // measured = last measured output angle, degrees (quantised to 0.1)
    // dt       = seconds since the previous call
    // returns  = commanded velocity, deg/s (will be saturated by the plant)
    virtual double update(double target, double measured, double dt) = 0;

    // Optional: reset any internal state (integrators, filters, etc).
    virtual void reset() {}
};
