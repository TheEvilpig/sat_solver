#include <random>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <vector>


int get_random_1_to_n(int n) {
    static std::random_device rd;
    static std::mt19937 gen(rd()); // Initialized only ONCE across the entire program
    
    std::uniform_int_distribution<int> distrib(1, n); // Range can adapt dynamically
    return distrib(gen);
}

//fisher-yates shuffle
std::vector<int> get_random_sample_from_range(int start, int end, int n) {
    std::vector<int> v;
    for(int i = start; i <= end; i++){
        v.push_back(i);
    }

    int lastIdx = end - start;

    for(int i = lastIdx; i >= 0; i--){
        int ran = get_random_1_to_n(i + 1) - 1;
        int temp = v[i];
        v[i] = v[ran];
        v[ran] = temp;
    }

    std::vector<int> sample;
    for(int i = 0; i < n && i <= lastIdx; i++){
        sample.push_back(v[i]);
    }

    return sample;
}

std::ofstream get_outfile(const std::filesystem::path& file_path){
    if (file_path.has_parent_path()) {
        std::filesystem::create_directories(file_path.parent_path());
    }

    std::ofstream outfile(file_path);

    return outfile;
}

std::string generate_random_clause(int num_v, int max_v){
    std::string s;
    for(int i = 0; i < num_v; i++){
        if(get_random_1_to_n(2) == 1)
            s += "-";
            
        s += std::to_string(get_random_1_to_n(max_v));
        s += " ";
    }
    return s + "0";
}

//generate fully random k_sat allowing tautologies, repeted vars etc.
void generate_random_ksat_loose(int v, int c, int k, const std::filesystem::path& file_path){

    if(v <= 0){std::cerr << "variables must be greater than 0\n"; return;}
    if(c <= 0){std::cerr << "clauses must be greater than 0\n"; return;}
    if(k <= 0){std::cerr << "clauses cannot be empty\n";return;}

    std::ofstream fout = get_outfile(file_path);

    if (!fout.is_open()){
        std::cerr << "couldnt open \"" << file_path << "\"\n";
        return;
    }

    fout << "p cnf " << v << " " << c << "\n";
    for(int i = 0; i < c; i++){
        fout << generate_random_clause(k, v) << "\n";
    }
}

//generate a k_sat with no tautologies, repeated vars and all variables must be used
void generate_random_ksat_strict(int v, int c, int k, const std::filesystem::path& file_path){

    if(v <= 0){std::cerr << "variables must be greater than 0\n"; return;}
    if(c <= 0){std::cerr << "clauses must be greater than 0\n"; return;}
    if(k <= 0){std::cerr << "clauses cannot be empty\n";return;}
    if(v < k){std::cerr << "need at least k variables to fill a clause\n";return;}

    std::ofstream fout = get_outfile(file_path);

    if (!fout.is_open()){
        std::cerr << "couldnt open \"" << file_path << "\"\n";
        return;
    }

    int total_vars = c * k;

    if(total_vars < v){
        std::cerr << "too many variables to include all of them\n";
        return;
    }

    fout << "p cnf " << v << " " << c << "\n";
    
    //generate random indices for garounteed variables
    std::vector<int> sample = get_random_sample_from_range(0, total_vars - 1, v);
    std::vector<std::pair<int, int>> index_var_pairs;

    for(int i = 1; i <= v; i++){
        index_var_pairs.push_back(std::make_pair(sample[i-1], i));
    }

    sort(index_var_pairs.begin(), index_var_pairs.end());


    size_t ivp_idx = 0;

    for(int i = 0; i < c; i++){
        std::string s;
        std::unordered_set<int> used;
        std::vector<int> clause_vars = get_random_sample_from_range(1, v, k);
        size_t cv_idx = 0;

        // TODO: fix this, it can place multiple of same var in one clause if the random sample had it before 
        // the pregenerated index
        for(int j = 0; j < k; j++){
            int next;
            if(ivp_idx < index_var_pairs.size() && i * k + j == index_var_pairs[ivp_idx].first){
                used.insert(index_var_pairs[ivp_idx].second);
                next = index_var_pairs[ivp_idx].second;

                ivp_idx++;
            } else{
                while(cv_idx < clause_vars.size() && used.contains(clause_vars[cv_idx]))
                    cv_idx++;

                if(cv_idx >= clause_vars.size()){
                    std::cerr << "ran out of sample variables for clause " << i << "\n";
                    return;
                }
                used.insert(clause_vars[cv_idx]);
                next = clause_vars[cv_idx];
                cv_idx++;
            }

            if(get_random_1_to_n(2) == 1)
                s += "-";
            s += std::to_string(next);
            s += " ";
        }

        fout << s << "0\n";
    }
}

//generate a random k sat with no repeated variables in each clause
//this gets rid of easy "trivialities" such as repeated variables and 
//tautologies(a clause containing a and !a is forced to be true)
//this is the most standard sat K-SAT problem and is used in most research
void generate_random_ksat(int v, int c, int k, const std::filesystem::path& file_path){

    if(v <= 0){std::cerr << "variables must be greater than 0\n"; return;}
    if(c <= 0){std::cerr << "clauses must be greater than 0\n"; return;}
    if(k <= 0){std::cerr << "clauses cannot be empty\n";return;}
    if(v < k){std::cerr << "need at least k variables to fill a clause\n";return;}

    std::ofstream fout = get_outfile(file_path);

    if (!fout.is_open()){
        std::cerr << "couldnt open \"" << file_path << "\"\n";
        return;
    }

    fout << "p cnf " << v << " " << c << "\n";

    for(int i = 0; i < c; i++){
        std::string s;

        //random sample method garountees no repeats
        std::vector<int> clause_vars = get_random_sample_from_range(1, v, k);

        for(int var : clause_vars){
            if(get_random_1_to_n(2) == 1)
                s += "-";
            s += std::to_string(var);
            s += " ";
        }

        fout << s << "0\n";
    }
}

// Encodes (n + 1) pigeons into n holes in DIMACS CNF format
void generate_pidgeon_hole(int n, const std::filesystem::path& file_path) {
    if (n <= 0) {
        std::cerr << "n must be greater than 0\n";
        return;
    }

    std::ofstream fout(file_path); // Adjust if using custom get_outfile(file_path)
    if (!fout.is_open()) {
        std::cerr << "couldnt open \"" << file_path << "\"\n";
        return;
    }

    int num_pigeons = n + 1;
    int num_holes = n;
    int num_vars = num_pigeons * num_holes;

    // Helper lambda to map (pigeon, hole) -> DIMACS 1-based variable index
    auto var = [num_holes](int pigeon, int hole) {
        return (pigeon * num_holes) + hole + 1;
    };

    // Calculate exact clause count
    uint64_t alo_clauses = num_pigeons;
    uint64_t amo_clauses = static_cast<uint64_t>(num_holes) * (num_pigeons * (num_pigeons - 1) / 2);
    uint64_t total_clauses = alo_clauses + amo_clauses;

    // Print DIMACS Header
    fout << "p cnf " << num_vars << " " << total_clauses << "\n";

    // 1. Every pigeon must be in AT LEAST ONE hole
    for (int i = 0; i < num_pigeons; ++i) {
        for (int j = 0; j < num_holes; ++j) {
            fout << var(i, j) << " ";
        }
        fout << "0\n";
    }

    // 2. No two pigeons can share the SAME hole
    for (int j = 0; j < num_holes; ++j) {
        for (int i1 = 0; i1 < num_pigeons; ++i1) {
            for (int i2 = i1 + 1; i2 < num_pigeons; ++i2) {
                fout << "-" << var(i1, j) << " -" << var(i2, j) << " 0\n";
            }
        }
    }
}

std::string generate_ground_truth_sat(int v, int c, int k, const std::filesystem::path& file_path){
    if(v <= 0){std::cerr << "variables must be greater than 0\n"; return"";}
    if(c <= 0){std::cerr << "clauses must be greater than 0\n"; return"";}
    if(k <= 0){std::cerr << "clauses cannot be empty\n";return"";}


    return "";
}