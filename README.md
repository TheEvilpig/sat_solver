# SAT Solver

A C++ SAT solver implementing and comparing multiple solving strategies (naive brute force, 
bitmask-based, and DPLL), with generators for producing test CNF instances.

## What's a SAT problem?
SAT problems or boolean satisfiability problems ask wether a formula (conisting of boolean variables and logical operators) is satisfiable, or able to equal true under some interpretation(assignment of truch values to each variable). SAT problems is the textbook example of an NP Hard problem and most NP Hard problems can be converted to a SAT problem.

## Solvers implemented
- **Naive brute force** — tries all 2^n assignments
- **Bitmask solve** — instead of assigning each variable one at a time compares strings encoding every possible assignment of that variable to find satisfiability.
- **Optimized bitmask solve** — optimizes bitmask solve by generating strings on the fly as to not use excessive memory or time acccesing higher level RAM chaches.
- **DPLL** — recursive DPLL with unit propagation to fixpoint and a trail for backtracking

## Generators
- Random k-SAT (loose/strict/standard)
- Pigeonhole principle instances
- Ground truth SAT instances (guaranteed satisfiable)

## Building
Requires `g++` with C++20 support.

This compiles all source files and produces an executable called `solver`.

## Running
You'll be prompted to either solve an existing CNF file or generate a new one:

- **Solve**: enter the path to a CNF file (DIMACS format) and pick a solver (`naive_solve`/`ns`, `bitmask_solve`/`bs`, `optimized_bitmask_solve`/`obs`, `dpll_solve`/`ds`, or `2SAT_solve`/`2ss`).
- **Write**: enter a file path and a problem type to generate (`random`/`r`, `random_loose`/`rl`, `random_strict`/`rs`, `pidgeon_hole`/`ph`, or `ground_truth`/`gt`), then follow the additional prompts for that type (number of variables, clauses, etc).

After each operation you'll be asked whether to quit or keep going.