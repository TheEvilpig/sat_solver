#include <string>
#include <vector>
#include <unordered_set>
#include <deque>

// solves the SAT problem using brute force
// works the same for any type SAT problem
// mechanically limited to 63 variables due to size of long long
// will probably run out of time way before that though
// returns a string with the solution or "no solution" if there is no solution
std::string naive_solve(int v, std::vector<int> &args){

    if(args.size() == 0) 
        return "error: empty arg";

    long long mx = 1LL << v;

    int len = args.size();

    for(long long i = 0; i < mx; ++i){
        bool fin = true;
        
        int j = 0;

        while(j < len){
            bool b = false;

            while(args[j] != 0){
                b |= ((args[j] % 2)) == ((i >> (args[j] / 2 - 1)) & 1);

                ++j;
            }

            if(!b){
                fin = false;
                break;
            }

            ++j;
            fin &= b;
        }

        if(fin){
            std:: string s;
            for(int x = 0; x < v; x++){
                s += (i >> x) & 1 ? "true " : "false ";
            }

            return "solution: " + s;
        }
    }

    return "no solution";
}


// optimized solution using bitmasking
// each variable gets a bitset representing assignments where that variable is true
// by comparing these bitsets with eachother we need to only "one pass" of the args
// the final result is a bitset representing all assignments that satisfy the SAT problem
// obviously this has terrible memory usage as memory grows at 2^v
// but you get a 64x speedup over the naive solution
// due to 64 bit modern CPUs allowing you to do 64 operations at once
// technically limited to 63 variables due to size of long long 
// but were gonna cap it early because the memory will get out of hand way before
std::string bitmask_solve(int v, std::vector<int> &args){
    if(v > 28){
        return "error: too many variables, memory usage may exceed 2GB";
    }

    if(args.size() == 0) 
        return "error: empty arg";

    long long assignments = 1LL << v;
    uint64_t words = (assignments + 63) / 64;

    std::vector<std::vector<uint64_t>> true_assignments(v, std::vector<uint64_t>(words));

    //generate the bitsets for each variable
    for(int i = 0; i < v ; ++i){
        if(i <= 5){
            long long period = 1LL << (i);
            uint64_t chunk = ((1ULL << (period)) - 1) << (period);
            uint64_t word = 0ULL;
            for(int j = 0; j < 64 / (period * 2); ++j){
                if(j != 0){
                    word <<= 2 * period;
                }
                word |= chunk;
            }
            for(uint64_t j = 0; j < words; ++j){
                true_assignments[i][j] = word;
            }
        } else{
            uint64_t period = 1ULL << (i - 6);
            uint64_t all0s = 0ULL;
            uint64_t all1s = ~0ULL;
            
            for(uint64_t j = 0; j < words; j++){
               if((j / period) % 2 == 0){
                    true_assignments[i][j] = all0s;
                } else{
                    true_assignments[i][j] = all1s;
                }
            }
        }

    }

    //identity for AND, will AND with each clause to get the final result
    std::vector<uint64_t> result(words, ~0ULL);

    for(size_t i = 0; i < args.size(); ++i){

        //identity for OR, will OR with each variable to get result of the clause
        std::vector<uint64_t> clause_result(words, 0ULL);

        while(args[i] != 0){
            int var = args[i] / 2 - 1;
            bool is_pos = args[i] % 2;

            if(is_pos){
                for(uint64_t j = 0; j < words; j++){
                    if(result[j] == 0) continue; // skip words already known unsatisfiable
                    clause_result[j] |= true_assignments[var][j];
                }
            } else{
                for(uint64_t j = 0; j < words; j++){
                    if(result[j] == 0) continue; 
                    clause_result[j] |= ~true_assignments[var][j];
                }
            }

            ++i;
        }
        
        for(uint64_t j = 0; j < words; ++j){
            if(result[j] == 0) continue; 
            result[j] &= clause_result[j];
        }
    }

    //check for and find solution
    bool isSolution = false;
    uint64_t solutionWord = 0ULL;
    uint64_t solutionBit = 0ULL;

    for(uint64_t j = 0; j < words; ++j){
        if(result[j] != 0){
            isSolution = true;
            solutionWord = j;

            //isolate the rightmost 1 bit in result[j]
            solutionBit = result[j] & -result[j];
            break;
        }
    }

    if(isSolution){
        std::string s;
        for(int x = 0; x < v; ++x){
            true_assignments[x][solutionWord] & solutionBit ? s += "true " : s += "false ";
        }

        return "solution: " + s;
    } 

    return "no solution";
}

// the origional bimask solution is thoeretically optimal but practically slow
// this is because it uses a lot of space so has to allocate to lower levels of cache and memory
// incedentally, it also has terrible memory usage
// we will solve both of these issues while using the same bitset approach
// this time we will generate each word of the bitset on the fly instead of storing all of them
// this will reduce memory usage to O(1) and speed up the solution 
// still limited to 63 variables...
std::string optimized_bitmask_solve(int v, int c, std::vector<int> &args){

    // since were iterating in 64 bit words, I dont want to worry about the case where
    // we cant fill a whole word. So let's just pass that case to the naive solution...
    if(v <= 6){
        return naive_solve(v, args);
    }

    uint64_t assignments = 1ULL << v;
    uint64_t words = (assignments + 63) / 64;
    

    //precompute constant results
    size_t curr_clause = 0;
    std::vector<uint64_t> precompute_results(c, 0ULL);
    for(size_t i = 0; i < args.size(); ++i){
        while(args[i] != 0){
            int var = args[i] / 2 - 1;
            bool is_pos = args[i] % 2;

            switch(var){
                case 0:
                    precompute_results[curr_clause] |= is_pos ? 
                    0xAAAAAAAAAAAAAAAAULL : ~0xAAAAAAAAAAAAAAAAULL;
                    break;
                case 1:
                    precompute_results[curr_clause] |= is_pos ? 
                    0xCCCCCCCCCCCCCCCCULL : ~0xCCCCCCCCCCCCCCCCULL;
                    break;
                case 2:
                    precompute_results[curr_clause] |= is_pos ? 
                    0xF0F0F0F0F0F0F0F0ULL : ~0xF0F0F0F0F0F0F0F0ULL;
                    break;
                case 3:
                    precompute_results[curr_clause] |= is_pos ? 
                    0xFF00FF00FF00FF00ULL : ~0xFF00FF00FF00FF00ULL;
                    break;
                case 4:
                    precompute_results[curr_clause] |= is_pos ? 
                    0xFFFF0000FFFF0000ULL : ~0xFFFF0000FFFF0000ULL;
                    break;
                case 5:
                    precompute_results[curr_clause] |= is_pos ? 
                    0xFFFFFFFF00000000ULL : ~0xFFFFFFFF00000000ULL;
                    break;
                default:
                    break;
            }

            ++i;
        }
        curr_clause++;
    }

    for(size_t i = 0; i < words; ++i){

        size_t current_clause = 0;

        //identity for AND, will AND with each clause to get the final result
        uint64_t result = ~0ULL;

        for(size_t j = 0; j < args.size(); ++j){

            //identity for OR, will OR with each variable to get result of the clause
            uint64_t clause_result = precompute_results[current_clause];

            while(args[j] != 0){

                int var = args[j] / 2 - 1;
                bool is_pos = args[j] % 2;

                if(var > 5){
                    //make the word for each variable on the fly instead of storing it
                    int shift = var - 6;
                    uint64_t word = (((i >> shift) & 1) == 0) ? 0ULL : ~0ULL;

                    clause_result |= is_pos ? word : ~word;
                }

                ++j;
            }

            current_clause++;

            result &= clause_result;

            if(result == 0){
                break;
            }
        }

        //check if we found solution
        if(result != 0){
            result = result & -result; //isolate the rightmost 1 bit in result

            std::string s;

            for(int x = 0; x < v; ++x){
                uint64_t word;
                switch(x){
                    case 0:
                        word = 0xAAAAAAAAAAAAAAAAULL;
                        break;
                    case 1:
                        word = 0xCCCCCCCCCCCCCCCCULL;
                        break;
                    case 2:
                        word = 0xF0F0F0F0F0F0F0F0ULL;
                        break;
                    case 3:
                        word = 0xFF00FF00FF00FF00ULL;
                        break;
                    case 4:
                        word = 0xFFFF0000FFFF0000ULL;
                        break;
                    case 5:
                        word = 0xFFFFFFFF00000000ULL;
                        break;
                    default:
                        int shift = x - 6;
                        word = (((i >> shift) & 1) == 0) ? 0ULL : ~0ULL;
                }

                result & word ? s += "true " : s += "false ";
            }

            return "solution: " + s;
        }
    }

    return "no solution";
}

// 2-SATs are special because they are solvable in poly-time
// the method will be to convert the roblem into an implication graph
// if you have a 2 variable clause: (a v b) thats true, then ~a implies b and ~b implies a
// both of these will become directed edges in our implication graph.
// the variables and their negations are vertices.
// the SAT is unsolveable iff a variable implies its negation and vice versa
// so we use a SCC algorithm like Kosaraju's(or Tarjan's) 
// and check that no variable is in the same SCC as its negation
// (v is limited by call stack size since I used a normal dfs algo)
void dfs_2SAT_f(
    std::vector<std::unordered_set<int>> &g, 
    std::vector<bool> &vis,
    std::deque<int> &s,
    int idx
){
    vis[idx] = true;
    for(int x : g[idx]){
        if(!vis[x])
            dfs_2SAT_f(g, vis, s, x);
    }
    s.push_front(idx);
}

void dfs_2SAT_r(
    std::vector<std::unordered_set<int>> &g, 
    std::vector<int> &comp,
    int co,
    int idx
){
    comp[idx] = co;
    for(int x : g[idx]){
        if(comp[x] == -1)
            dfs_2SAT_r(g, comp, co, x);
    }
    
}

std::string solve_2SAT(int v, std::vector<int> &args){
    //construct implication graph and its transpose
    std::vector<std::unordered_set<int>> impl(2 * v);
    std::vector<std::unordered_set<int>> impl_t(2 * v);

    for(size_t i = 0; i < args.size(); ++i){
        std::vector<int> v;
        while(args[i] != 0){
            int var = args[i] / 2 - 1;
            bool is_pos = args[i] % 2;

            v.push_back(2 * var + (is_pos ? 0 : 1));

            ++i;
        }

        int a, b;

        if(v.size() == 1){
            a = b = v[0];
        } else{
            a = v[0];
            b = v[1];
        }

        impl[a ^ 1].insert(b);
        impl[b ^ 1].insert(a);

        impl_t[a].insert(b ^ 1);
        impl_t[b].insert(a ^ 1);
    }


    // kosaraju constructing a comparison list of sccs
    // for a, b if comp[a] == comp[b] they are in the same scc
    // if comp[a] < comp[b] a comes first topologically
    // islands dont follow this rule but they will never get compared so whatever
    std::vector<bool> vis(2 * v, false);
    std::deque<int> stk;
    std::vector<int> comp(2 * v, -1);

    for(int i = 0; i < 2 * v; i++)
        if(!vis[i])
            dfs_2SAT_f(impl, vis, stk, i);
    
    int co = 0;

    while(!stk.empty()){
        int curr = stk.front();
        stk.pop_front();
        if(comp[curr] == -1)
            dfs_2SAT_r(impl_t, comp, co++, curr);
    }

    std::string s;

    for(int i = 0; i < v; i++){
        if(comp[2 * i] ==  comp[2 * i + 1])
            return "no solution";
        s += comp[2 * i] > comp[2 * i + 1] ? "true " : "false ";
    }

    return "solution: " + s;
}