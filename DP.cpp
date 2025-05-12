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

void get_pure_literals(cl_map& clauses, cl_l_t& clauses_l, cl& pure_literals) {
    for (auto i = clauses_l.begin(); i != clauses_l.end(); i ++) {
        for (auto j = clauses[*i].begin(); j != clauses[*i].end(); j ++) {
            for (int k = 1; k < j -> size(); k ++) {
                if ((*j)[k] != 0) {
                    if (pure_literals[k] == -2) {
                        pure_literals[k] = (*j)[k];
                    } else if (pure_literals[k] + (*j)[k] == 0) {
                        pure_literals[k] = 0;
                    }
                }
            }
        }
    }
    pure_literals[0] = 0;
    for (int i = 1; i < pure_literals.size(); i ++) {
        if (pure_literals[i] == -2) {
            pure_literals[i] = 0;
        }
        if (pure_literals[i] != 0) {
            pure_literals[0] ++;
        }
    }
}

cl_t::iterator delete_clause(cl_t& set_of_clauses, cl_t::iterator clause) {
    return set_of_clauses.erase(clause);
}

bool delete_literal(cl_map& clauses, const cl& clause, short literal, cl_l_t& clauses_l) {
    cl new_clause = clause;
    if (literal > 0) {
        new_clause[literal] = 0;
    } else {
        new_clause[-literal] = 0;
    }
    new_clause[0] --;
    clauses_l.insert(new_clause[0]);
    clauses[new_clause[0]].insert(new_clause);
    if (new_clause[0] == 0) {
        return 1;
    }

    return 0;
}

short dp_first_rule(cl_map& clauses, cl_l_t& clauses_l) {
    queue<short> literals;
    short response;

    for (auto i = clauses[1].begin(); i != clauses[1].end(); i ++) {
        for (int j = 1; j < i -> size(); j ++) {
            if ((*i)[j] != 0) {
                literals.push(j * (*i)[j]);
            }
        }
    }
    
    while(!literals.empty()) {
        for (auto i = clauses_l.begin(); i != clauses_l.end(); i ++) {
            for (cl_t::iterator j = clauses[*i].begin(); j != clauses[*i].end(); ) {
                bool iterator_was_changed = 0;
                if (literals.front() < 0) {
                    if (*i == 1 && (*j)[-literals.front()] == -1) {
                        break;
                    }else if ((*j)[-literals.front()] == -1) {
                        int clause_size = (*j)[0];
                        j = delete_clause(clauses[*i], j);
                        iterator_was_changed = 1;
                    } else if ((*j)[-literals.front()] == 1) {
                        response = delete_literal(clauses, *j, -literals.front(), clauses_l);
                        j = delete_clause(clauses[*i], j);
                        iterator_was_changed = 1;
                    }
                } else {
                    if (*i == 1 && (*j)[literals.front()] == 1) {
                        break;
                    }else if ((*j)[literals.front()] == 1) {
                        int clause_size = (*j)[0];
                        j = delete_clause(clauses[*i], j);
                        iterator_was_changed = 1;
                    } else if ((*j)[literals.front()] == -1) {
                        response = delete_literal(clauses, *j, -literals.front(), clauses_l);
                        j = delete_clause(clauses[*i], j);
                        iterator_was_changed = 1;
                    }
                }

                if (!iterator_was_changed) {
                    j ++;
                }

                if (response == 1) {
                    return -1;
                }
            }
        }

        for (auto i = clauses_l.begin(); i != clauses_l.end();) {
            if (clauses[*i].size() == 0) {
                i = clauses_l.erase(i);
            } else {
                i ++;
            }
        }

        if (clauses_l.empty()) {
            return 1;
        }

        literals.pop();
    }
    
    clauses.erase(1);
    clauses_l.erase(1);

    return 0;
}

short dp_second_rule(cl_map& clauses, cl_l_t& clauses_l, cl& pure_literals) {
    vector<short> literals;
    for (int i = 1; i < pure_literals.size(); i ++) {
        if (pure_literals[i] != 0) {
            literals.push_back(i);
        }
    }
    for (auto i = clauses_l.begin(); i != clauses_l.end(); i ++) {
        for (auto j = clauses[*i].begin(); j != clauses[*i].end();) {
            bool changed_j = 0;
            for (int k = 1; k < j -> size(); k ++) {
                if ((*j)[k] != 0 && find(literals.begin(), literals.end(), k) != literals.end()) {
                    j = delete_clause(clauses[*i], j);
                    changed_j = 1;
                }
                if (j == clauses[*i].end()) {
                    break;
                }
            }
            if (!changed_j) {
                j ++;
            }
        }
    }

    for (auto i = clauses_l.begin(); i != clauses_l.end();) {
        if (clauses[*i].size() == 0) {
            i = clauses_l.erase(i);
        } else {
            i ++;
        }
    }

    if (clauses_l.empty()) {
        return 1;
    }

    if (*clauses_l.begin() == 0) {
        return -1;
    }

    for (int i = 0; i < pure_literals.size(); i ++) {
        pure_literals[i] = -2;
    }
    pure_literals[0] = 0;

    return 0;
}

bool DP(cl_map& clauses, cl_l_t& clauses_l) {
    cl pure_literals((*clauses[*clauses_l.begin()].begin()).size(), -2);
    pq_t PQ;
    cl_t new_clauses;
    cl_l_t::iterator it1 = clauses_l.begin();
    cl_t::iterator it2 = clauses[*it1].begin();

    short response = 0;
    while (!response) {
        while (*clauses_l.begin() == 1) {
            response = dp_first_rule(clauses, clauses_l);
            if (response == -1) {
                cout << "nesatisfiabil";
                print_execution_time();
            }
            if (response == 1) {
                cout << "satisfiabil";
                print_execution_time();
            }
        }
        
        get_pure_literals(clauses, clauses_l, pure_literals);
        while (pure_literals[0] > 0) {
            response = dp_second_rule(clauses, clauses_l, pure_literals);
            if (response == -1) {
                cout << "nesatisfiabil";
                print_execution_time();
            }
            if (response == 1) {
                cout << "satisfiabil";
                print_execution_time();
            }
            get_pure_literals(clauses, clauses_l, pure_literals);
        }
        pq_t pq_aux;
        PQ.swap(pq_aux);

        for (auto i = clauses_l.begin(); i != clauses_l.end(); i ++) {
            for (auto j = clauses[*i].begin(); j != clauses[*i].end(); j ++) {
                PQ.push(*j);
            }
        }
        
        while (!PQ.empty() && !(*clauses_l.begin() == 1)) {
            for (; it1 != clauses_l.end(); it1 ++) {
                for (; it2 != clauses[*it1].end(); it2 ++) {
                    if (clauses[PQ.top().size()].find(PQ.top()) != clauses[PQ.top().size()].end()) {
                        if (addable(PQ.top(), *it2)) {
                            cl new_clause;
                            add_clauses(PQ.top(), *it2, new_clause);
                            
                            if (new_clause[0] == 0) {
                                return 0;
                            }
                            
                            if (clauses[new_clause[0]].find(new_clause) == clauses[new_clause[0]].end() && new_clauses.find(new_clause) == new_clauses.end()) {
                                new_clauses.insert(new_clause);
                            } 
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
    }
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
    bool res = DP(clauses, clauses_l);
    if (res) {
        cout << "satisfiabil";
        print_execution_time();
    } else {
        cout << "nesatisfiabil";
        print_execution_time();
    }
}