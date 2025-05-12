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

bool addable(const cl& cl1, const cl& cl2) {
    for (int i = 1; i <= cl1.size(); i ++) {
        if (cl1[i] + cl2[i] == 0 && cl1[i] != 0) {
            return 1;
        }
    }

    return 0;
}

void add_clauses(const cl& cl1, const cl& cl2, cl& new_cl) {
    short changes = 0;
    new_cl.push_back(0);
    for (int i = 1; i < cl1.size(); i ++) {
        new_cl.push_back(cl1[i] + cl2[i]);
        if (new_cl[i] == 2 || new_cl[i] == -2) {
            new_cl[i] /= 2;
        }

        if (new_cl[i] != 0) {
            new_cl[0] ++;
        }
        
        if (new_cl[i] == 0 && cl1[i] != 0) {
            changes ++;
        }
    }

    if (changes > 1) {
        new_cl = cl1;
    }
}

void append_new_clauses (cl_map& clauses, cl_t& new_clauses, cl_l_t& clauses_l) {
    for (auto i = new_clauses.begin(); i != new_clauses.end(); i ++) {
        clauses[(*i)[0]].insert(*i);
        clauses_l.insert((*i)[0]);
    }
}

void append_new_clauses (pq_t& PQ, cl_t& new_clauses) {
    for (auto i = new_clauses.begin(); i != new_clauses.end(); i ++) {
        PQ.push(*i);
    }
}

bool resolution(cl_map &clauses, cl_l_t& clauses_l, int& number_of_clauses) {
    pq_t PQ;
    cl_t new_clauses;
    for (auto i = clauses_l.begin(); i != clauses_l.end(); i ++) {
        for (auto j = clauses[*i].begin(); j != clauses[*i].end(); j ++) {
            PQ.push(*j);
        }
    }
    
    while (!PQ.empty()) {
        for (auto i = clauses_l.begin(); i != clauses_l.end(); i ++) {
            for (auto j = clauses[*i].begin(); j != clauses[*i].end(); j ++) {
                if (addable(PQ.top(), *j)) {
                    cl new_clause;
                    add_clauses(PQ.top(), *j, new_clause);

                    if (new_clause[0] == 0) {
                        return 0;
                    }
            
                    if (clauses[new_clause[0]].find(new_clause) == clauses[new_clause[0]].end() && new_clauses.find(new_clause) == new_clauses.end()) {
                        new_clauses.insert(new_clause);
                    } 
                }
            }
        }
        PQ.pop();
        if (!new_clauses.empty()) {
            append_new_clauses(clauses, new_clauses, clauses_l);
            append_new_clauses(PQ, new_clauses);
            new_clauses.clear();
        }
    }
    return 1;
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
    bool res = resolution(clauses, clauses_l, number_of_clauses);
    if (res) {
        cout << "satisfiabil";
        print_execution_time();
    } else {
        cout << "nesatisfiabil";
        print_execution_time();
    }
}