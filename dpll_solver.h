#ifndef DPLL_SOLVER_H
#define DPLL_SOLVER_H

#include "parser.h"

#include <vector>
#include <deque>
#include <string>

// this is the final SAT solver before our (somewhat) optimal CDCL solver
// DPLL
// dpll is based on a recursive brute force search where we pick a variable v
// and then run dpll again on the version of the formula that has v true and v false
// before we do any of this however we do boolean constraint propagation
// which just means we take logical next steps for example if a variable is alone (unit literal)
// we set it to the value that would make that clause true, or if it's always or never negated (pure)
// then we set it so it's always true and then we make simplifications ex: a V b V true = true
// and repeat

// struct to safely store globals
struct DpllSolver
{
    std::vector<int> assignment; // 0 false, 1 true, 2 unassigned
    std::deque<int> history;
    std::vector<std::vector<int>> clauses;

    // Runs unit propagation. Returns false if a conflict (empty clause) is found.
    bool propagate();

    // Recursive DPLL search. Assumes clauses/assignment/history are already set up.
    bool dpll_solve();

    // Entry point: builds the clause database from parsed args, runs the solver,
    // and formats the result as a string.
    std::string dpll(int v, int c, std::vector<int> &args);
};

#endif // DPLL_H