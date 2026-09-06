#include "dpll_solver.h"

bool DpllSolver::propagate()
{
    // keep a flag for if a change was made, if a full loop makes no change, terminate.
    // look for singleton vars (all others false) and conflicts (all false)
    // break out of clauses early if you find true
    // once you find an unassigned singleton assign it to whatever satisfies that clause
    // and add it to history and flag the change

    bool change = true;
    while (change) {
        change = false;
        for (size_t i = 0; i < clauses.size(); ++i) {
            int last_unassigned = -1;
            int num_unassigned = 0;
            bool true_found = false;
            for (int l : clauses[i]) {
                if (assignment[l / 2] == l % 2) {
                    true_found = true;
                    break;
                }

                if (assignment[l / 2] == 2) {
                    last_unassigned = l;
                    num_unassigned++;
                }
            }

            if (!true_found && num_unassigned == 0)
                return false;

            if (!true_found && num_unassigned == 1) {
                history.push_front(last_unassigned / 2);
                assignment[last_unassigned / 2] = last_unassigned % 2;
                change = true;
            }
        }
    }

    return true;
}

bool DpllSolver::dpll_solve()
{
    // propagate and check for conflicts

    // check for next unassigned variable
    // if you can't find one then the problem is satisfied

    // mark the current size of history as the index of divergence
    // assign the next variable as true, add that to history, and call dpll_solve() again
    // if this doesn't come out true, revert history and repeat but assign false

    if (!propagate())
        return false;

    bool unassigned_found = false;
    int unassigned = -1;
    for (size_t i = 1; i < assignment.size(); ++i) {
        if (assignment[i] == 2) {
            unassigned_found = true;
            unassigned = static_cast<int>(i);
            break;
        }
    }

    if (!unassigned_found)
        return true;

    history.push_front(unassigned);
    assignment[unassigned] = 1;

    if (dpll_solve()) {
        return true;
    }

    while (history.front() != unassigned) {
        assignment[history.front()] = 2;
        history.pop_front();
    }

    assignment[unassigned] = 0;

    if (dpll_solve()) {
        return true;
    }

    return false;
}

std::string DpllSolver::dpll(int v, int c, std::vector<int> &args)
{
    clauses = parse_args_to_vector(c, args);
    history.clear();
    assignment.assign(v + 1, 2);
    if (dpll_solve()) {
        std::string s;
        if (!assignment.empty()) {
            for (auto i = std::next(assignment.begin()); i != assignment.end(); ++i) {
                s += *i == 0 ? " false" : " true";
            }
        }
        return "solution:" + s;
    }

    return "no solution";
}