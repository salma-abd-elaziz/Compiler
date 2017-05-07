map<string, vector<pair<int, int>>> firsts;
string original_non_terminal;
bool test = true;

/// Handle non-immediate LF bt taking common factors from all productions for
/// each non-terminal.
void take_commons (vector<string>::iterator n_ter) {
    string nont = *n_ter;
    if (nonterminal[nont].size() <= 1) return;
    map<string, vector<int>> f;
    int index = 0;
    // Just take commons regardless it is terminal or non-terminal.
    for (vector<vector<string>>::iterator it = nonterminal[nont].begin();
                                  it != nonterminal[nont].end(); it++, index++)
        f[*(it->begin())].push_back(index);
    // Add new non-terminal and remove unneeded productions.
    for (map<string, vector<int>>::iterator it = f.begin(); it != f.end();
                                                                        it++) {
        if ((it->second).size() <= 1) continue;
        // Create new non-terminal.
        string new_n_ter = nont + "'";
        while (nonterminal.count(new_n_ter)) new_n_ter += "'";
        nonter.insert(n_ter + 1, new_n_ter);
        // Create productions for new non-terminal.
        for (int i : it->second) {
            if (nonterminal[nont][i].size() == 1) {
                vector<string> s;
                s.push_back(EPSILON);
                nonterminal[new_n_ter].push_back(s);
            } else {
                vector<string> s(nonterminal[nont][i].begin() + 1,
                                 nonterminal[nont][i].end());
                nonterminal[new_n_ter].push_back(s);
            }
        }
        // Remove unneeded productions.
        for (vector<int>::iterator i = it->second.end() - 1;
                                                  i >= it->second.begin(); i--)
            nonterminal[nont].erase(nonterminal[nont].begin() + (*i));
        // Add the new production to the original non-terminal's productions.
        vector<string> s;
        s.push_back(it->first);
        s.push_back(new_n_ter);
        nonterminal[nont].push_back(s);
        take_commons (find(nonter.begin(), nonter.end(), new_n_ter));
    }
}

/// Get firsts of all productions for a specific non-terminal.
void get_first (string nont, int pro_no, string previous_non_terminal) {
    int index = 0;
    if (nonterminal.count(nont) == 0 || nont == previous_non_terminal) return;
    for (vector<vector<string>>::iterator it = nonterminal[nont].begin();
                                it != nonterminal[nont].end(); it++, index++) {
        // If first is terminal -> add to map and continue.
        if (terminal.count(*(it->begin()))) firsts[*(it->begin())]
                                          .push_back(make_pair(pro_no, index));
        else { // Else recurse to get firsts of recursive non-terminals.
            vector<string>::iterator s = it->begin();
            s++;
            if (*(it->begin()) == EPSILON && s != it->end())
                get_first(*s, pro_no, nont);
            else if (*(it->begin()) == original_non_terminal) {
                test = false;
                firsts[*(it->begin())].push_back(make_pair(pro_no, -2));
            }
            else get_first(*(it->begin()), pro_no, nont);
        }
    }
}

/// Handle non-immediate LF by substituting with first nonterminals if needed.
void non_immediate_LF (string n_ter, bool flag) {
    if (nonterminal[n_ter].size() <= 1 || flag) return;
    bool recursion = true;
    test = true;
    firsts.clear();
    original_non_terminal = n_ter;
    int index = 0;
    for (vector<vector<string>>::iterator it = nonterminal[n_ter].begin();
                               it != nonterminal[n_ter].end(); it++, index++) {
        if (terminal.count(*(it->begin())))
            firsts[*(it->begin())].push_back(make_pair(index, -1));
        else { // Else recurse to get firsts of recursive non-terminals.
            if (*(it->begin()) == n_ter || *(it->begin()) == EPSILON) continue;
            else get_first(*(it->begin()), index, n_ter);
        }
    }
    // Substitute for non-terminals in each production if needed.
    for (map<string, vector<pair<int, int>>>::iterator it = firsts.begin();
                                                    it != firsts.end(); it++) {
        if (it->second.size() <= 1 && test) // No need for substitution.
            continue;
        for (vector<pair<int, int>>::iterator i = it->second.begin();
                                                  i != it->second.end(); i++) {
            if (i->second == -1) continue;
            string s = *(nonterminal[n_ter][i->first].begin());
            nonterminal[n_ter][i->first].erase(nonterminal[n_ter][i->first]
                                                                     .begin());
            vector<string> temp(nonterminal[n_ter][i->first].begin(),
                                nonterminal[n_ter][i->first].end());
            bool fir = true;
            // Substitue with all productions of the non-terminal in the
            // specified production.
            for (vector<vector<string>>::iterator ve = nonterminal[s].begin();
                                            ve != nonterminal[s].end(); ve++) {
                if (nonterminal.count(*(ve->begin()))) recursion = false;
                if (fir) {
                    fir = false;
                    if (!(ve->size() == 1 && *(ve->begin()) == EPSILON))
                        nonterminal[n_ter][i->first].insert(
                        nonterminal[n_ter][i->first].begin(), ve->begin(),
                        ve->end());
                    continue;
                }
                vector<string> t;
                if (!(ve->size() == 1 && *(ve->begin()) == EPSILON))
                    t.insert(t.begin(), ve->begin(), ve->end());
                t.insert(t.end(), temp.begin(), temp.end());
                nonterminal[n_ter].push_back(t);
            }
        }
        take_commons(find(nonter.begin(), nonter.end(), n_ter));
    }
    non_immediate_LF(n_ter, recursion);
}

/// Handle immediate LF and non-immediate LF.
void handle_productions (vector<string>::iterator n_ter) {
    string s = *n_ter;
    take_commons(n_ter);
    non_immediate_LF(s, false);
}

/// Handle left Factoring in grammar (LF).
void left_factoring () {
    for (vector<string>::iterator it = nonter.end() - 1; it >= nonter.begin();
                                                                          it--)
        handle_productions(it);
}

/// Handle left recursion in grammar (LR).
void eliminate_left_recursion () {
    // Iterate on nonterminal and substitute with all nonterminals before
    // current nonterminal.
    int initialSize = nonter.size();
    for (int i = 0; i < initialSize; i++) {
        string Ai = nonter.at(i);
        map<string,vector<vector<string>>>::iterator itr1 = nonterminal
                                                                     .find(Ai);
        vector<vector<string>> AiProductions = itr1->second;
        for (int j = 0; j < i; j++) {
            string Aj = nonter.at(j);
            vector<vector<string>> AjProductions = nonterminal
                                                             .find(Aj)->second;
            for (unsigned int k = 0; k < AiProductions.size(); k++) {
                vector<string> productionOfAi = AiProductions.at(k);
                // Replace this with Aj productions.
                if (productionOfAi.at(0).compare(Aj) == 0) {
                    productionOfAi.erase(productionOfAi.begin()); // Delete Aj.
                    for (vector<string> productionOfAj : AjProductions) {
                        for (string s : productionOfAi) {
                            // Delete epsilon before concatenate.
                            if (productionOfAj.at(0).compare(EPSILON) == 0)
                                productionOfAj.erase(productionOfAj.begin());
                            productionOfAj.push_back(s);
                        }
                        // Here productionOfAj become ready to push in
                        // Aiproductions.
                        AiProductions.push_back(productionOfAj);
                    }
                    // Delete old production from Ai.
                    AiProductions.erase(AiProductions.begin()+k);
                    // Assume left factoring only one production begin with Aj.
                    break;
                }
            }
        }
        // Eliminate left recursion if found.
        vector<string> alpha;
        bool leftRecursion = false;
        for (vector<vector<string>>::iterator vitr=AiProductions.begin();
                                        vitr != AiProductions.end() ; vitr++) {
            vector<string>concList = *vitr;
            if (concList.at(0).compare(Ai) == 0) { // Here left recursion.
                // Get alpha and beta.
                concList.erase(concList.begin()); // Delete first element which
                                                  // cause left recursion.
                alpha = concList;
                AiProductions.erase(vitr); // Beta.
                leftRecursion = true;
                break;
            }
        }
        // Eliminate left recursion if found.
        if (leftRecursion) { // Found left recursion.
            // Modify original nonterminal.
            string newNontreminal = Ai + "'";
            while (nonterminal.count(newNontreminal)) newNontreminal += "'";
            vector<vector<string>> beta;
            // Append new nonterminal in beta.
            for (vector<string> concList : AiProductions) {
                concList.push_back(newNontreminal);
                beta.push_back(concList);
            }
            // Add extra nonterminal.
            alpha.push_back(newNontreminal);
            vector<string> epsilon;
            epsilon.push_back(EPSILON);
            vector<vector<string>> newDisjList;
            newDisjList.push_back(alpha);
            newDisjList.push_back(epsilon);
            itr1->second = beta;
            nonterminal.insert(make_pair(newNontreminal,newDisjList));
            nonter.push_back(newNontreminal);
            leftRecursion = false;
        }
        // There is not left recursion, change Ai productions in map only.
        else
            itr1->second = AiProductions;
    }
}

/// Dealing with LF and LR.
void handle_grammar() {
    left_factoring();
    eliminate_left_recursion();
}
