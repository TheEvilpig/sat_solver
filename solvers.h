#ifndef SOLVERS_H
#define SOLVERS_H

#include <string>
#include <vector>
#include <unordered_set>
#include <deque>

std::string naive_solve(int v, std::vector<int> &args);
std::string bitmask_solve(int v, std::vector<int> &args);
std::string optimized_bitmask_solve(int v, int c, std::vector<int> &args);
std::string solve_2SAT(int v, std::vector<int> &args);

#endif