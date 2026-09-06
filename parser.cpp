#include <algorithm>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>

// returns a pair of the type of SAT problem and the arguments
// arguments are each integers with the rightmost bit being the sign 
// and the rest of the bits being the variable number
std::pair<int, std::vector<int>> parseInput(int v, int c, std::istream& in){
    if(v <= 0){
        std::cerr << "error: no variables\n";
        return std::make_pair(-1, std::vector<int>());
    }

    if(c <= 0){
        std::cerr <<  "error: no clauses\n";
        return std::make_pair(-1, std::vector<int>());
    }

    std::vector<int> args;
    int len = 0;
    int max_clause_len = 0;

    for(int i = 0; i < c; ++i){
        std::string line;

        std::getline(in, line);
        while(line.empty() || line[0] == 'c'){
            std::getline(in, line);
        }

        std::stringstream ss(line);

        int value;
        int clause_len = 0;
        while(ss >> value){

            int var = std::abs(value);
            int is_pos = (value > 0);
            args.push_back((var << 1) | is_pos);
            ++len;

            //max clause length is the type of SAT problem...
            clause_len++;
            if(var == 0){
                if(clause_len - 1 > max_clause_len){
                    max_clause_len = clause_len - 1;
                }
                clause_len = 0;
            }

            if(args[len-1] == 0 && (len == 1 || args[len-2] == 0)){
                std::cerr << "error: empty clause\n";
                return std::make_pair(-1, std::vector<int>());
            }
        }
    }   

    return std::make_pair(max_clause_len, args);
}

std::vector<std::vector<int>> parse_args_to_vector(int c, std::vector<int> &args){
    std::vector<std::vector<int>> v(c);
    
    size_t i = 0;
    for(size_t j = 0; j < args.size(); ++j){
        while(args[j] != 0){
            v[i].push_back(args[j]);
            ++j;
        }
        ++i;
    }

    return v;
}