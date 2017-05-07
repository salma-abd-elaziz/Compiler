#ifndef THOMPSONCONSTRUCTION_H
#define THOMPSONCONSTRUCTION_H
#include <iostream>
#include <fstream>  /* Stream class to both read and write from/to files. */
#include <vector>
#include <map>
#include <regex>
#include <set>

#define Epsilon "epsilon"

using namespace std;

class range
{
    public:
        int start;
        int end_;
        bool operator == (const range &rhs) const {
            return (rhs.start == start) && (rhs.end_ == end_);
        }
        bool operator < (const range &rhs) const {
            return (rhs.start < start) && (rhs.end_ < end_);
        }
};

class State
{
    public:
        int id;
        string acceptance;
        vector<pair<int,range>> transition;
        vector<int> epslon_closure; //the E_closure of the state
        vector<int> equivalent_NFA_states;
        bool marked = false;
};

class Automata
{
    public:
        State start_state;
        State end_state;
};

#endif // THOMPSON’SCONSTRUCTION_H

class ThompsonConstruction
{
    public:
        map<int, State> final_NFA;
        int unique_id = 1; // Initially 1 (0 reserved for combination).
        std::smatch m;
        /* Regular definition. */
        std::regex letter;
        std::regex digit;
        /* Regular expressions. */
        std::regex id;
        std::regex digits;
        std::regex num;
        std::regex keywords;
        std::regex punctuations;
        std::regex arithop;
        std::regex assi;
        std::regex relop;
        map<string, vector<range>> alphabet;
        set<range> basic_alph;
        ThompsonConstruction ();
        Automata create_automata (range r);
        Automata disjunction (Automata first, Automata second);
        Automata disjunction_more (vector<Automata> automatas);
        Automata concatenation (Automata first, Automata second);
        Automata closure (Automata base);
        void read_file ();
        void execute ();
};
