#include "parser.h"
#include "solvers.h"
#include "dpll_solver.h"
#include "cnf_generator.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>
#include <cstdint>
#include <cstdlib>
#include <chrono>
#include <deque>
#include <unordered_set>
#include <algorithm>

// TODO: implement the generate_* functions themselves — this just wires up the prompts and dispatch
void writeToFile(const std::string& filepath, std::string& p_type) {

    auto askVCK = [](int& v, int& c, int& k) {
        std::cout << "enter number of variables: ";
        std::cin >> v;
        std::cout << "enter number of clauses: ";
        std::cin >> c;
        std::cout << "enter variables per clause (k): ";
        std::cin >> k;
        std::cin.ignore();
    };

    if(p_type == "random_loose" || p_type == "rl"){
        int v, c, k;
        askVCK(v, c, k);
        generate_random_ksat_loose(v, c, k, filepath);
    } else if(p_type == "random_strict" || p_type == "rs"){
        int v, c, k;
        askVCK(v, c, k);
        generate_random_ksat_strict(v, c, k, filepath);
    } else if(p_type == "random" || p_type == "r"){
        int v, c, k;
        askVCK(v, c, k);
        generate_random_ksat(v, c, k, filepath);
    } else if(p_type == "pidgeon hole" || p_type == "ph"){
        int n;
        std::cout << "enter number of holes (n): ";
        std::cin >> n;
        std::cin.ignore();

        generate_pidgeon_hole(n, filepath);
    } else if(p_type == "ground_truth" || p_type == "gt"){
        int v, c, k;
        askVCK(v, c, k);
        std::cout << generate_ground_truth_sat(v, c, k, filepath) << "\n";
    } else{
        std::cerr << "invalid problem type.\n";
        return;
    }

}

void solveFile(const std::string& filename, const std::string& solve_type) {
    std::ifstream infile(filename);
    if(!infile.is_open()){
        std::cerr << "could not open file: " << filename << "\n";
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();
    int t = 1000000;

    std::string line;

    while(std::getline(infile, line)){
        //c is comment line
        if(line.empty() || line[0] == 'c')
            continue;

        //SAT inputs start with a p
        if(line[0] == 'p'){
            char p;
            char cnf[4];
            int vars;
            int clauses;

            std::stringstream ss(line);

            ss >> p >> cnf >> vars >> clauses;

            //parse input, parse info contains the type of SAT problem and the arguments
            std::cout << "parsing...\n";
            std::pair<int, std::vector<int>> parse_info = parseInput(vars, clauses, infile);
            std::vector<int> parse_args = parse_info.second;
            int sat_type = parse_info.first;

            //time to parse the input
            std::cout << "time: " << (std::chrono::high_resolution_clock::now() - start).count() / t << "\n";
            start =  std::chrono::high_resolution_clock::now();

            //time to solve the SAT problem

            if(solve_type == "bitmask_solve" || solve_type == "bs"){
                std::cout << bitmask_solve(vars, parse_args) << "\n";
            } else if(solve_type == "naive_solve" || solve_type == "ns"){
                std::cout << naive_solve(vars, parse_args) << "\n";
            } else if(solve_type == "optimized_bitmask_solve" || solve_type == "obs"){
                std::cout << optimized_bitmask_solve(vars, clauses, parse_args) << "\n";
            } else if(solve_type == "dpll_solve" || solve_type == "ds"){
                DpllSolver dpll_solver = {};
                std::cout << dpll_solver.dpll(vars, clauses, parse_args) << "\n";
            } else if(solve_type == "2SAT_solve" || solve_type == "2ss"){
                if(sat_type > 2){
                    std::cerr << "must be a 2 sat.\n";
                    return;
                }
                std::cout << solve_2SAT(vars, parse_args) << "\n";
            } else{
                std::cerr << "invalid solve type.\n";
                return;
            }

            std::cout << "time: " << (std::chrono::high_resolution_clock::now() - start).count() / t << "\n";
            start =  std::chrono::high_resolution_clock::now();
        }
    }

    infile.close();
}

int main() {

    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::string choice;

    while(true){
        std::cout << "solve or write to file? (solve/write, or quit): ";
        std::getline(std::cin, choice);

        if(choice == "quit" || choice == "q"){
            break;
        } else if(choice == "solve" || choice == "s"){
            std::string filename;
            std::cout << "enter filename: ";
            std::getline(std::cin, filename);

            std::string solve_type;
            std::cout << "enter solve type: ";
            std::getline(std::cin, solve_type);

            solveFile(filename, solve_type);
        } else if(choice == "write" || choice == "w"){
            std::string filepath;
            std::cout << "enter file path to write to: ";
            std::getline(std::cin, filepath);

            std::string prob_type;
            std::cout << "enter type of SAT problem to generate: ";
            std::getline(std::cin, prob_type);

            writeToFile(filepath, prob_type);
        } else {
            std::cerr << "invalid choice, please enter 'solve', 'write', or 'quit'.\n";
            continue;
        }

        std::string again;
        std::cout << "do you want to quit? (y/n): ";
        std::getline(std::cin, again);

        if(again == "y" || again == "yes"){
            break;
        }
    }

    return 0;
}