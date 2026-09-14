// ============================================================================
// Check your solution. Run this after Part A's decoder has produced
// ../data/*.csv. One command, two checks:
//
//   CHECK 1 -- does plant.hpp match the real bench data?
//     Replays the real u_commanded (from your decoded ../data/*.csv) through
//     your Plant::step(), and compares the result to the real y_measured.
//     Low error means your model actually explains the bench actuator.
//
//   CHECK 2 -- does controller.hpp control plant.hpp well?
//     Closes the loop between your controller.hpp and your plant.hpp over a
//     large move, a direction reversal, and a small on-centre trim, and
//     reports settling / overshoot for each.
//
// Check 2 is only meaningful once Check 1 passes -- a controller that looks
// perfect against the wrong plant tells you nothing about the real actuator.
//
// Build & run (from inside this checks/ folder):
//     c++ -std=c++17 check_solution.cpp -o check_solution
//     ./check_solution
//     python3 plot_checks.py     # plots the CSVs above, real vs. model overlaid
// ============================================================================

#include "../controller.hpp"
#include "../plant.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// CHECK 1: plant.hpp vs the real data
// ---------------------------------------------------------------------------
namespace check1 {

struct Row { double t, u, y; };

std::vector<Row> loadCsv(const std::string& path) {
    std::vector<Row> rows;
    std::ifstream f(path);
    std::string line;
    std::getline(f, line);  // header
    while (std::getline(f, line)) {
        std::istringstream ss(line);
        std::string a, b, c;
        std::getline(ss, a, ',');
        std::getline(ss, b, ',');
        std::getline(ss, c, ',');
        if (a.empty()) continue;
        rows.push_back({std::stod(a), std::stod(b), std::stod(c)});
    }
    return rows;
}

bool runOne(const std::string& name) {
    const std::string inPath  = "../data/" + name + ".csv";
    const std::string outPath = name + ".csv";

    std::vector<Row> rows = loadCsv(inPath);
    if (rows.empty()) {
        std::printf("  %-14s -- %s not found. Run Part A's decoder first.\n",
                    name.c_str(), inPath.c_str());
        return false;
    }

    Plant plant;
    std::ofstream out(outPath);
    out << "t,u_commanded,y_measured,y_predicted\n";

    double maxErr = 0.0, sumSq = 0.0;
    double prevT = rows[0].t;
    for (size_t i = 0; i < rows.size(); ++i) {
        const double dt = (i == 0) ? (rows.size() > 1 ? rows[1].t - rows[0].t : 0.005)
                                   : rows[i].t - prevT;
        prevT = rows[i].t;
        const double yPred = plant.step(rows[i].u, dt);
        out << rows[i].t << "," << rows[i].u << "," << rows[i].y << "," << yPred << "\n";
        const double err = yPred - rows[i].y;
        maxErr = std::max(maxErr, std::fabs(err));
        sumSq += err * err;
    }
    const double rms = std::sqrt(sumSq / rows.size());
    const bool ok = maxErr < 1.0;  // a fair model should track to within ~1 deg
    std::printf("  %-14s max err = %7.3f deg   rms = %7.3f deg   %s\n",
               name.c_str(), maxErr, rms, ok ? "-- looks right" : "-- doesn't match");
    return ok;
}

}  // namespace check1

// ---------------------------------------------------------------------------
// CHECK 2: controller.hpp vs plant.hpp, closed loop
// ---------------------------------------------------------------------------
namespace check2 {

struct Phase { const char* label; double target; double seconds; };
const std::vector<Phase> kProfile = {
    {"large move",    20.0, 6.0},
    {"reversal",      12.0, 5.0},
    {"on-centre trim",13.5, 4.0},
};

void run() {
    const double dt = 0.005;
    Plant plant;
    Controller controller;
    controller.reset();

    std::ofstream out("controller.csv");
    out << "t,target,cmd,measured\n";

    double measured = 0.0;
    double tGlobal = 0.0;
    for (const Phase& phase : kProfile) {
        const double startMeasured = measured;
        const int steps = static_cast<int>(phase.seconds / dt);
        double peak = measured;
        double settleTime = -1.0;
        double t = 0.0;
        const bool increasing = phase.target > startMeasured;
        for (int i = 0; i < steps; ++i) {
            const double cmd = controller.update(phase.target, measured, dt);
            measured = plant.step(cmd, dt);
            out << tGlobal << "," << phase.target << "," << cmd << "," << measured << "\n";
            peak = increasing ? std::max(peak, measured) : std::min(peak, measured);
            if (std::fabs(measured - phase.target) > 0.1) settleTime = t;
            t += dt;
            tGlobal += dt;
        }
        const double overshoot = increasing ? std::max(0.0, peak - phase.target)
                                            : std::max(0.0, phase.target - peak);
        const double endErr = measured - phase.target;
        std::printf("  %-16s target=%6.2f  end=%6.2f (err %+.2f)  overshoot=%.2f deg"
                   "  settled by t=%.2fs into phase\n",
                   phase.label, phase.target, measured, endErr, overshoot,
                   settleTime < 0 ? 0.0 : settleTime);
    }
}

}  // namespace check2

int main() {
    std::printf("CHECK 1 -- plant.hpp against the real bench data\n");
    bool plantOk = true;
    plantOk &= check1::runOne("step_test");
    plantOk &= check1::runOne("reversal_test");
    plantOk &= check1::runOne("deadband_test");
    std::printf("\n");

    std::printf("CHECK 2 -- controller.hpp against plant.hpp"
               " (move -> reversal -> small trim)\n");
    if (!plantOk)
        std::printf("  (plant.hpp doesn't match the real data yet -- this check is only"
                   " testing your controller\n   against your OWN model, not the real"
                   " actuator. Fix Check 1 first.)\n");
    check2::run();

    std::printf("\nWrote checks/*.csv -- plot with: python3 plot_checks.py\n");
    return 0;
}
