#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <queue>
#include <set>
#include <unordered_map>
#include <string>
#include <sstream>
#include <ctime>
#include <new>
using namespace std;

#define cl vector<short>
#define cl_t set<cl>
#define cl_l_t set<short>
#define cl_map unordered_map<int, cl_t>
#define pq_t priority_queue<cl, vector<cl>, greater<cl>>

time_t* starting_time = nullptr, *ending_time = nullptr;

void start_time() {
    if (starting_time == nullptr) {
        starting_time =  new time_t(time(nullptr));
    }
}

time_t get_execution_time() {
    if (ending_time == nullptr) {
        ending_time =  new time_t(time(nullptr));
    }
    return *ending_time - *starting_time;
}

void print_execution_time() {
    time_t seconds = get_execution_time();
    cout << "\nRunning time: " << seconds / 60 << " minutes " << seconds % 60 << " seconds(" << seconds << " seconds)";
    exit(0);
}

void out_of_memory_handler() {
    cerr << "Out of memory\nsatisfiable\n";
    print_execution_time();
    abort();
}

void print_clause(const cl& clause) {
    cout << "{";
    for (int i = 1; i < clause.size(); i ++) {
        if (clause[i] != 0) {
            cout << clause[i] * i << " ";
        }
    }
    cout <<"}\n";
}

short is_satisfiable(cl& clause, cl& values) {
    int sum = 0;
    for (int i = 0; i < clause.size(); i ++) {
        if (clause[i] != 0 && clause[i] == values[i]) {
            sum += clause[i];
        }
    }
    if (sum == clause[0]) {
        return 1;
    }
    if (sum == -clause[0]) {
        return -1;
    }
    return 0;
}

short satisfied_clauses(cl_map& clauses, cl_l_t& clauses_l, cl& assigned_values) {
    int number_of_satisfied_clauses = 0, number_of_clauses = 0;
    for (auto i = clauses_l.begin(); i != clauses_l.end(); i ++) {
        for (auto j = clauses[*i].begin(); j != clauses[*i].end(); j ++) {
            int number_of_false_literals = 0;
            bool found_true_literal = 0;
            number_of_clauses ++;
            for (int k = 1; k < j -> size(); k ++) {
                if (!found_true_literal && (*j)[k] != 0 && assigned_values[k] == (*j)[k]) {
                    number_of_satisfied_clauses ++;
                    found_true_literal = 1;
                } else if ((*j)[k] != 0 && assigned_values[k] == -(*j)[k]) {
                    number_of_false_literals ++;
                }
            }
            if (number_of_false_literals == (*j)[0]) {
                return -1;
            }
        }
    }
    if (number_of_satisfied_clauses == number_of_clauses) {
        return 1;
    }
    return 0;
}

void unit_propagation(cl_map& clauses, cl_l_t clauses_l, cl& assigned_values) {
     for (auto i = clauses_l.begin(); i != clauses_l.end(); i ++) {
         for (auto j = clauses[*i].begin(); j != clauses[*i].end(); j ++) {
            int number_of_false_literals = 0, position_of_unassigned_literal = -1;
            for (int k = 1; k < j -> size(); k ++) {
                if ((*j)[k] != 0 && assigned_values[k] != 0 && assigned_values[k] == -(*j)[k]) {
                    number_of_false_literals ++;
                } else if ((*j)[k] != 0 && assigned_values[k] == 0) {
                    position_of_unassigned_literal = k;
                }
            }
            if (number_of_false_literals == (*j)[0] - 1) {
                assigned_values[position_of_unassigned_literal] = (*j)[position_of_unassigned_literal];
            }
        }
    }
}

short pick_literal(cl& assigned_values) {
    for (int i = 1; i < assigned_values.size(); i ++) {
        if (assigned_values[i] == 0) {
            return i;
        }
    }

    return -1;
}

bool DPLL(cl_map& clauses, cl_l_t& clauses_l, cl& assigned_values) {
    cl original_assigned_values = assigned_values;
    short response = 0;
    unit_propagation(clauses, clauses_l, assigned_values);
    response = satisfied_clauses(clauses, clauses_l, assigned_values);

    if (response == -1) {
        assigned_values = original_assigned_values;
        return 0;
    }
    if (response == 1) {
        return 1;
    }
    
    short x = pick_literal(assigned_values);

    if (x == -1) {
        assigned_values = original_assigned_values;
        return 0;
    }
    
    assigned_values[x] = 1;
    response =  DPLL(clauses, clauses_l, assigned_values);
    
    if (response == 1) {
        assigned_values = original_assigned_values;
        return 1;
    }
    
    assigned_values[x] = -1;
    response =  DPLL(clauses, clauses_l, assigned_values);

    if (response == 1) {
        assigned_values = original_assigned_values;
        assigned_values[x] = 0;
        return 1;
    }
    
    assigned_values = original_assigned_values;
    return 0;
}

int main() {
    set_new_handler(out_of_memory_handler);
    ifstream f("./input.cnf");
    string comments, aux;
    int number_of_variables, number_of_clauses;
    
    char c = f.peek();
    while (f.peek() == 'c') {
        getline(f, comments);
    }

    getline(f, aux);
    
    istringstream iss(aux);
    iss >> aux >> aux >> number_of_variables >> number_of_clauses;
    
    cl_map clauses;
    cl_l_t clauses_l;

    cl assigned_values(number_of_clauses + 1, 0);
    
    for (int i = 0; i < number_of_clauses; i ++) {
        cl clause(number_of_variables + 1, 0);
        int n;
        f >> n;
        
        while(n != 0) {
            if (n > 0) {
                clause[n] = 1;
            }
            if (n < 0) {
                clause[-n] = -1;
            }
            clause[0] ++;
            f >> n;
        }

        clauses[clause[0]].insert(clause);
        clauses_l.insert(clause[0]);
    }
    f.close();
    
    start_time();
    bool res = DPLL(clauses, clauses_l, assigned_values);
    if (res) {
        cout << "satisfiabil";
        print_execution_time();
    } else {
        cout << "nesatisfiabil";
        print_execution_time();
    }
}