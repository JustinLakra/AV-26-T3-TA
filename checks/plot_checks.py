#!/usr/bin/env python3
"""
Plot the outputs of check_solution.cpp: your plant.hpp overlaid on the real
data (Check 1), and your controller's closed-loop response (Check 2).

Run from inside this checks/ folder, after check_solution:
    c++ -std=c++17 check_solution.cpp -o check_solution
    ./check_solution
    python3 plot_checks.py            # show the plots interactively
    python3 plot_checks.py --save     # also write PNGs here

Needs: matplotlib  (pip install matplotlib)
"""

import argparse
import csv
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))

PLANT_TESTS = [
    ("step_test.csv", "Check 1: step test -- plant.hpp vs real data"),
    ("reversal_test.csv", "Check 1: reversal test -- plant.hpp vs real data"),
    ("deadband_test.csv", "Check 1: deadband test -- plant.hpp vs real data"),
]


def load(path, cols):
    out = {c: [] for c in cols}
    with open(path, newline="") as f:
        for row in csv.DictReader(f):
            for c in cols:
                out[c].append(float(row[c]))
    return out


def plot_plant_checks(plt, save):
    for fname, title in PLANT_TESTS:
        path = os.path.join(HERE, fname)
        if not os.path.exists(path):
            print(f"skipping {fname} (not found -- run check_solution first)")
            continue
        d = load(path, ["t", "u_commanded", "y_measured", "y_predicted"])

        fig, (ax_u, ax_y) = plt.subplots(2, 1, sharex=True, figsize=(9, 6))
        fig.suptitle(title)

        ax_u.plot(d["t"], d["u_commanded"], lw=1, color="tab:gray")
        ax_u.set_ylabel("u_commanded\n(deg/s)")
        ax_u.grid(True, alpha=0.3)

        ax_y.plot(d["t"], d["y_measured"], lw=1.5, label="real y_measured")
        ax_y.plot(d["t"], d["y_predicted"], lw=1, ls="--", label="your plant.hpp")
        ax_y.set_ylabel("y (deg)")
        ax_y.set_xlabel("t (s)")
        ax_y.legend(loc="best")
        ax_y.grid(True, alpha=0.3)

        fig.tight_layout()
        if save:
            out = os.path.join(HERE, fname.replace(".csv", ".png"))
            fig.savefig(out, dpi=120)
            print(f"wrote {out}")


def plot_controller_check(plt, save):
    path = os.path.join(HERE, "controller.csv")
    if not os.path.exists(path):
        print("skipping controller.csv (not found -- run check_solution first)")
        return
    d = load(path, ["t", "target", "cmd", "measured"])

    fig, (ax_y, ax_cmd) = plt.subplots(2, 1, sharex=True, figsize=(9, 6))
    fig.suptitle("Check 2: controller.hpp holding target through move / reversal / trim")

    ax_y.plot(d["t"], d["target"], lw=1, ls="--", color="tab:gray", label="target")
    ax_y.plot(d["t"], d["measured"], lw=1.5, label="measured")
    ax_y.set_ylabel("angle (deg)")
    ax_y.legend(loc="best")
    ax_y.grid(True, alpha=0.3)

    ax_cmd.plot(d["t"], d["cmd"], lw=1, color="tab:orange")
    ax_cmd.set_ylabel("cmd\n(deg/s)")
    ax_cmd.set_xlabel("t (s)")
    ax_cmd.grid(True, alpha=0.3)

    fig.tight_layout()
    if save:
        out = os.path.join(HERE, "controller.png")
        fig.savefig(out, dpi=120)
        print(f"wrote {out}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--save", action="store_true", help="write PNGs here instead of showing them")
    args = ap.parse_args()

    try:
        import matplotlib.pyplot as plt
    except ImportError:
        sys.exit("matplotlib not found -- run:  pip install matplotlib")

    plot_plant_checks(plt, args.save)
    plot_controller_check(plt, args.save)

    if not args.save:
        plt.show()


if __name__ == "__main__":
    main()
