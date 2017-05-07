#ifndef SUBSETCONSTRUCTION_H
#define SUBSETCONSTRUCTION_H
#include <queue>
#include <stack>
#include <algorithm>
#include "ThompsonConstruction.h"

using namespace std;

class DFA_state
{
    public:
        string acceptance;
        vector<int> equivalent_NFA_states;
        int id;
};

class SubsetConstruction
{
    public:
        SubsetConstruction ();
        void get_DFA ();
        int find_acceptance (vector<int> states);
        map<int, vector<pair<int,string>>> DFA;
        void print_NFA ();
    private:
        vector<int> move (DFA_state state, int input);
        void get_e (vector<int> states);
        bool check_existance(vector<int> next_state, DFA_state state_to_check);
};

#endif // SUBSETCONSTRUCTION_H
