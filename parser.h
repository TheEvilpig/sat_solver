#ifndef PARSER_H
#define PARSER_H

#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>

std::pair<int, std::vector<int>> parseInput(int v, int c, std::istream& in);
std::vector<std::vector<int>> parse_args_to_vector(int c, std::vector<int> &args);

#endif