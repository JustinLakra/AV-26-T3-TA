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
        (void)dt;
        double error = target - measured;
        double kp = 1.0; // TODO: replace with your design
        return kp * error;
    }

    void reset() override {
        // TODO: reset any internal state here, if you have any.
    }
};
