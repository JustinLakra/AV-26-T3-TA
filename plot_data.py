#!/usr/bin/env python3
"""
Plot the three decoded bench tests so you can start looking at the actuator's
behaviour. Run Part A first (decode data/*.log -> data/*.csv); this script
reads those CSVs. Not required -- use Excel, MATLAB, gnuplot, or your own
script if you prefer. The CSVs are plain text with a header row: columns are
t, u_commanded, y_measured.

Compare the output against the pre-plotted data/*.png files to check your
decode before you rely on it.

Usage:
    python3 plot_data.py            # show the plots interactively
    python3 plot_data.py --save     # also write PNGs as data/*_yours.png
                                     # (kept separate from the pre-plotted
                                     # data/*.png so you always have both to
                                     # compare)

Needs: matplotlib  (pip install matplotlib)
"""

import argparse
import csv
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
DATA = os.path.join(HERE, "data")

TESTS = [
    ("step_test.csv", "Step test - constant velocity command, then stop"),
    ("reversal_test.csv", "Reversal test - square-wave velocity, repeated reversals"),
    ("deadband_test.csv", "Deadband test - small velocity sweep near centre"),
]


def load(path):
    t, u, y = [], [], []
    with open(path, newline="") as f:
        for row in csv.DictReader(f):
            t.append(float(row["t"]))
            u.append(float(row["u_commanded"]))
            y.append(float(row["y_measured"]))
    return t, u, y


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--save", action="store_true", help="write PNGs into data/")
    args = ap.parse_args()

    try:
        import matplotlib.pyplot as plt
    except ImportError:
        sys.exit("matplotlib not found -- run:  pip install matplotlib\n"
                 "(or open the CSVs in data/ with a spreadsheet instead)")

    for fname, title in TESTS:
        path = os.path.join(DATA, fname)
        if not os.path.exists(path):
            print(f"skipping {fname} (not found -- decode the .log files in Part A first)")
            continue
        t, u, y = load(path)

        fig, (ax_u, ax_y) = plt.subplots(2, 1, sharex=True, figsize=(9, 6))
        fig.suptitle(title)

        ax_u.plot(t, u, lw=1)
        ax_u.set_ylabel("u_commanded\n(deg/s)")
        ax_u.grid(True, alpha=0.3)

        ax_y.plot(t, y, lw=1)
        ax_y.set_ylabel("y_measured\n(deg)")
        ax_y.set_xlabel("t (s)")
        ax_y.grid(True, alpha=0.3)

        fig.tight_layout()

        if args.save:
            out = os.path.join(DATA, fname.replace(".csv", "_yours.png"))
            fig.savefig(out, dpi=120)
            print(f"wrote {out}")

    if not args.save:
        plt.show()


if __name__ == "__main__":
    main()
