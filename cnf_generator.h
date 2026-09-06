#ifndef CNF_GENERATOR_H
#define CNF_GENERATOR_H

#include <random>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <vector>

void generate_random_ksat_loose(int v, int c, int k, const std::filesystem::path& file_path);

void generate_random_ksat_strict(int v, int c, int k, const std::filesystem::path& file_path);

void generate_random_ksat(int v, int c, int k, const std::filesystem::path& file_path);

void generate_pidgeon_hole(int n, const std::filesystem::path& file_path);

std::string generate_ground_truth_sat(int v, int c, int k, const std::filesystem::path& file_path);

#endif