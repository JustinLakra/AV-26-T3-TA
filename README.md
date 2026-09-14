# Steering control technical assessment

You're given three CAN bus captures from bench-testing an unknown steering
actuator - the plant, which is just the control-systems term for the thing
being controlled (as opposed to the controller, the thing you write to drive
it). Your task is to recover the bench data from the logs, work out how the
actuator behaves, and write a C++ controller that holds a commanded angle.

## Getting Started

Fork this repo and clone your fork. Work there, not on the original.

You'll need a C++17 compiler (`c++`/g++/clang) on `PATH`. `python3` +
matplotlib are optional, for the plotting scripts. This works out of the box
on Linux and macOS. **On Windows, use WSL** — the build commands in this
README assume a Unix-style toolchain (`c++ -std=c++17 ...`) that plain
PowerShell/cmd doesn't have by default.

## What's in this package

```
controller_interface.hpp   -- the interface your controller must implement (don't edit)
controller.hpp             -- to fill in (this is what you submit)
plant.hpp                  -- to fill in (your model of the actuator, from Part B)
written_responses.txt      -- to fill in (Parts A, B and D)
Question-A.cc              -- the Part A decode task (this is where your decoder goes)
SteeringBench.dbc          -- CAN signal definitions for the logs (Part A)
plot_data.py               -- plots your decoded data/*.csv against data/*.png (optional)
data/                      -- bench data: what Part A reads and writes
  step_test.log       -- constant velocity command, then stop     (CAN capture)
  reversal_test.log   -- square-wave velocity, repeated direction reversals
  deadband_test.log   -- small velocity sweep near the operating point
  step_test.png       -- the same three tests, pre-plotted, so you can
  reversal_test.png      check your decode looks right before you rely on it
  deadband_test.png
  step_test.csv       -- your decoded output, written by Question-A.cc
  reversal_test.csv
  deadband_test.csv
checks/                    -- optional: everything for checking plant.hpp/controller.hpp
  check_solution.cpp    -- see "Checking your solution" below
  plot_checks.py
```

## YOUR TASK

### Part A - recover the data (decode the CAN logs)

The task is explained in **`Question-A.cc`** 

### Part B - identify controller behaviour

Work out what the actuator does and capture it as `plant.hpp` - a working
model, just fill in `step()`. Your reasoning goes in `written_responses.txt`.

The plant is a first-order system (commanded rate in, angle out, with some
lag) plus one nonlinearity near the operating point.

- Estimate the plant's gain and time constant from `step_test`.
- Look at `deadband_test` near zero commanded rate - is the nonlinearity a
  deadzone or backlash? Mention the specific evidence and from which plot -
  name the plot and describe the shape, don't just assert a conclusion.
- Estimate the size of that nonlinearity as best you can from the data.

You don't need to hunt for every possible nonlinearity - just the one that's
visibly present.


### Part C - control

Implement `class Controller : public IController` (see
`controller_interface.hpp`) in the file `controller.hpp`.

Your `update(target, measured, dt)` is called once per control tick and
returns a commanded velocity in deg/s. It only sees the target angle, the last
measured angle, and the timestep - it never sees the plant's internal state,
and you can't use the plant model at runtime.

It has to hold a target angle through:

- a large move,
- a direction reversal,
- a small on-centre correction.

Implement a PID controller (proportional-integral-derivative). Tune `kp`,
`ki`, and `kd` for clean settling behaviour without inducing oscillation -
consider anti-windup on the integral term. Write design choices in
`written_responses.txt`.

### Part D - reflection (answer in written_responses.txt)

What didn't work, and why? We want to see your thought process!

## Checking your solution

Once Part A's decoder has produced `data/*.csv`, everything for checking
`plant.hpp` and `controller.hpp` lives in `checks/`. Build and run from
inside that folder:

```
cd checks
c++ -std=c++17 check_solution.cpp -o check_solution
./check_solution
python3 plot_checks.py     # optional: plot what it found
```

- **Check 1** replays the real `u_commanded` from your decoded CSVs through
  your `plant.hpp` and compares it to the real `y_measured` - this tells you
  whether your model actually explains the bench actuator. Written to
  `checks/step_test.csv`, `reversal_test.csv`, `deadband_test.csv`.
- **Check 2** closes the loop between your `controller.hpp` and your
  `plant.hpp` over a large move, a reversal, and a small on-centre trim, and
  reports settling and overshoot for each. Written to `checks/controller.csv`.

Check 2 only means something once Check 1 is solid - a controller that looks
perfect against the wrong plant tells you nothing about the real actuator.
Once both look right, push your `plant.hpp` parameters up and down and re-run
Check 2 - the real actuator won't match your estimate exactly, so gains that
only work for one exact model aren't good gains.

## Resources:
https://www.csselectronics.com/pages/can-bus-simple-intro-tutorial
https://www.csselectronics.com/pages/can-dbc-file-database-intro
https://www.geeksforgeeks.org/electronics-engineering/control-system-tutorial/
https://www.youtube.com/watch?v=USH75nuHV6w
https://www.youtube.com/watch?v=NSDAsqJHsuc

