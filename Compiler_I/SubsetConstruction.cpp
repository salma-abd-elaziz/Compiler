#include "SubsetConstruction.h"
#include <iostream>
#define ID_START 1
#define START_ID 0
#define DOES_NOT_ACCEPT -1

using namespace std;

vector<int> result;
ThompsonConstruction NFA;

SubsetConstruction::SubsetConstruction ()
{
    NFA.execute();
}

void SubsetConstruction::get_DFA ()
{
    int id = ID_START;
    vector<DFA_state> temp;
    vector<int> v;
    State s0 = NFA.final_NFA[0];
    v.push_back(s0.id);
    DFA_state DFA_s0;
    get_e(v);
    DFA_s0.equivalent_NFA_states.assign (result.begin(), result.end());
    DFA_s0.id = START_ID;
    temp.push_back(DFA_s0);
    for (unsigned int j = 0; j < temp.size(); ++j) {
        // T.
        DFA_state state = temp[j];
        vector<pair<int,string>> transition;
        transition.push_back(pair<int, string> (0, state.acceptance));
        for (set<range>::iterator it1 = NFA.basic_alph.begin();
                                          it1 != NFA.basic_alph.end(); ++it1) {
            range input = *it1;
            for (int i = input.start; i <= input.end_; i++) {
                get_e(move(state, i));
                string input_string;
                input_string = (char)i;
                if (!result.empty()) {
                    vector<int> next_state;
                    next_state.assign(result.begin(), result.end());
                    bool found = false;
                    // Check if next_state in Dstate or not.
                    for (vector<DFA_state>::iterator it2 = temp.begin();
                                                    it2 != temp.end(); ++it2) {
                        DFA_state state_to_check = *it2;
                        if (check_existance(next_state, state_to_check)) {
                            transition.push_back(pair<int, string>
                                            (state_to_check.id, input_string));
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        DFA_state new_state;
                        // Id.
                        new_state.id = id++;
                        // Equivalent_states.
                        new_state.equivalent_NFA_states
                                .assign(next_state.begin(), next_state.end());
                        // Priority- acceptance.
                        int state = find_acceptance(result);
                        if (state == DOES_NOT_ACCEPT)
                            new_state.acceptance = "";
                        else
                            new_state.acceptance.assign(NFA.final_NFA[state]
                            .acceptance.begin(), NFA.final_NFA[state]
                            .acceptance.end());
                        transition.push_back(pair<int, string> (new_state.id,
                                                                input_string));
                        temp.push_back(new_state);
                    }
                // No transition with that i/p.
                } else
                    transition.push_back(pair<int, string> (-1, input_string));
            }
        }
        // Enter state in DFA.
        DFA[state.id].assign(transition.begin(), transition.end());
    }
}

vector<int> SubsetConstruction::move (DFA_state state, int input)
{
    vector<int> result1;
    vector<int> equi = state.equivalent_NFA_states;
    for (vector<int>::iterator it = equi.begin(); it != equi.end(); ++it) {
        vector<pair<int, range>> transition;
        transition.assign((NFA.final_NFA[*it].transition).begin(), (NFA
                                            .final_NFA[*it].transition).end());
        for (vector<pair<int, range>> ::iterator it1 = transition.begin();
                                              it1 != transition.end(); ++it1)
            if ((it1->second).start <= input && (it1->second).end_ >= input)
                result1.push_back(it1->first);
    }
    return result1;
}

void SubsetConstruction::get_e (vector<int> states)
{
    result.clear();
    stack <int> e_stack;
    // For each state in states vector.
    for (vector<int>::iterator it = states.begin(); it != states.end(); ++it) {
        e_stack.push(*it);
        // Add state to stack.
        result.push_back(*it);
    }
    while (!e_stack.empty()) {
        int state = e_stack.top();
        e_stack.pop();
        vector<int> e_closuer = NFA.final_NFA[state].epslon_closure;
        for (vector<int>::iterator it2 = e_closuer.begin();
                                                it2 != e_closuer.end(); ++it2)
            // Search for state in result vector.
            if (!(find(result.begin(), result.end(), *it2) != result.end())) {
                result.push_back(*it2);
                e_stack.push(*it2);
            }
    }
}

bool SubsetConstruction::check_existance (vector<int> next_state,
                                          DFA_state state_to_check)
{
    if (next_state.size() < state_to_check.equivalent_NFA_states.size() ||
        next_state.size() > state_to_check.equivalent_NFA_states.size())
        return false;
    sort(next_state.begin(), next_state.end());
    sort(state_to_check.equivalent_NFA_states.begin(), state_to_check.
                                                equivalent_NFA_states.end());
    for (unsigned int i = 0; i < next_state.size(); i++)
        if (next_state[i] !=  state_to_check.equivalent_NFA_states[i])
            return false;
    return true;
}

int SubsetConstruction::find_acceptance (vector<int> states)
{
    int acceptance_state = DOES_NOT_ACCEPT;
    for (unsigned int i = 0; i < states.size(); i++)
        if (NFA.final_NFA[states[i]].acceptance != ""
            && (states[i] > acceptance_state
            || acceptance_state == DOES_NOT_ACCEPT))
            acceptance_state = states[i];
    return acceptance_state;
}
