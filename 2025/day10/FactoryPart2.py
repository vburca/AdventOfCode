#!/usr/bin/env python3

import sys
import re
from pulp import LpProblem, LpVariable, lpSum, LpMinimize, LpInteger, PULP_CBC_CMD

def parse_line(line):
    # parse target
    target_match = re.search(r'{([^}]+)}', line)
    if not target_match:
        raise ValueError("No target found")
    target = list(map(int, target_match.group(1).split(',')))

    # parse buttons
    buttons = []
    for b in re.findall(r'\(([^)]+)\)', line):
        indices = list(map(int, b.split(',')))
        vec = [0] * len(target)
        for i in indices:
            if i < 0 or i >= len(target):
                raise ValueError(f"Button index {i} out of range")
            vec[i] = 1
        buttons.append(vec)

    return buttons, target

def solve_machine(buttons, target):
    B = len(buttons)
    D = len(target)

    # Create ILP problem
    prob = LpProblem("MinPresses", LpMinimize)

    # Variables: number of presses for each button
    x = [LpVariable(f"x{i}", lowBound=0, cat=LpInteger) for i in range(B)]

    # Constraints: final counters == target
    for j in range(D):
        prob += lpSum(buttons[i][j] * x[i] for i in range(B)) == target[j]

    # Objective: minimize total presses
    prob += lpSum(x)

    # Solve using default CBC solver
    solver = PULP_CBC_CMD(msg=False)
    status = prob.solve(solver)
    if status != 1:
        raise ValueError("No solution found")

    total_presses = sum(int(x[i].varValue) for i in range(B))
    return total_presses

def main():
    if len(sys.argv) < 2:
        print("Usage: python solver.py input.txt")
        sys.exit(1)

    filename = sys.argv[1]
    total = 0
    with open(filename) as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            buttons, target = parse_line(line)
            presses = solve_machine(buttons, target)
            total += presses

    print(total)

if __name__ == "__main__":
    main()
