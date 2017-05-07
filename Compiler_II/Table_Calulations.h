// Used variables.
smatch match;
regex proRule("#\\s+(.+)\\s+=\\s+(.+)");
regex extraProd("\\s*\\|\\s*(.+)");
map<string,vector<vector<string>>> nonterminal;
map<string,string> terminal;
map<string,vector<string>> first;
vector<string> nonter;
map<string, bool> contain_epslon;
map<pair<string,string>, vector<string>> table;

// Follow datastructures used.
struct followGp {
    vector<string> firstOf;            /* First of Non-Terminal {x1, x2, ..}
                                          to be added to followers of
                                          Non-Terminal yi. */
    vector<string> followOf;           /* Followers of Non-Terminal
                                          {x1, x2, ..} to be added to followers
                                           of Non-Terminal yi.*/
    vector<string> terminalFollowers;  /* Terminals followers of Non-Terminal
                                          y.*/
    bool followCalculated = false;
};

map<string, followGp> productionFollowers;   // After first iteration.
map<string, vector<string> > follow;         /* The final follower which is used
                                                to construct the parse table. */

/// Split string in specific chars.
vector<string> split (const string & s, string rgx_str = "\\s+") {
    vector<string> elems;       // Returned vector.
    regex rgx (rgx_str);
    sregex_token_iterator iter(s.begin(), s.end(), rgx, -1);
    sregex_token_iterator end;
    while (iter != end) {
        string s = *iter;
        if (s.length() != 0 && s != " ") {
            // Extract terminals from string.
            if (rgx_str.compare("\\s+") == 0 && s.length() > 2
                && s.at(0) == '\'' && s.at(s.length()-1) == '\'') {
                s = s.substr(1, s.length()-2);
                if (s.compare(EPSILON) != 0) terminal.insert(make_pair(s, ""));
            }
            elems.push_back(s);
        }
        iter++;
    }
    return elems;
}

/// Search for terminal in the form 'T'.
void extractTerminal (string line) {
    // Split line in | or space.
    vector<string> splited = split(line, "[|\\s+]");
    for (string s : splited)
        if (s.length() > 2 && s.at(0) == '\'' && s.at(s.length() - 1) == '\'')
            terminal.insert(make_pair(s.substr(1, s.length() - 2), ""));
}

void readFile() {
    string line;
    ifstream myfile("grammar.txt");
    if (myfile.is_open()) {
        string prevProd;
        vector<vector<string>> prevList;
        while(getline(myfile, line)) {
            // Match production rule line.
            if (regex_search(line, match, proRule)) {
                // Insert nonterminal associated with its productions.
                prevList.clear();
                prevProd = match[1];
                // Check if this nonterminal come before.
                map<string, vector<vector<string>>>::iterator mitr;
                mitr = nonterminal.find(prevProd);
                if (mitr != nonterminal.end()) prevList = mitr->second;
                // Else new prevList.
                vector<string> disjList = split(match[2], "[|]");
                for (string s : disjList) prevList.push_back(split(s));
                if (mitr != nonterminal.end()) mitr->second = prevList;
                else {
                    nonterminal.insert(make_pair(prevProd, prevList));
                    nonter.push_back(match[1]);
                }
            // Doesn't match production rule line then it's a complement.
            } else if(regex_search(line, match, extraProd)) {
                // Add extra productions to the previous nonterminal.
                vector<string> extraProd = split(match[1], "[|]");
                for (string s : extraProd)
                    if (s.length() != 0) prevList.push_back(split(s));
                nonterminal.erase(nonterminal.find(prevProd));
                nonterminal.insert(make_pair(prevProd, prevList));
            } else cout << "Error Line!!" << endl;
        }
        myfile.close();
    }
    else cout << "Unable to open file";
}

void calc_first (string nonterm, string prev_nonterm) {
    vector<string> firsts;
    for (vector<string> rule : nonterminal[nonterm]) {
        unsigned int counter = 0;
        string token = rule[counter];
        // Am I terminal?
        if (terminal.count(token) != 0) firsts.push_back(token);
        else if (token.compare(EPSILON) == 0) contain_epslon[nonterm] = true;
        else {
            // Nonterminal's first is added.
            bool has_epslon = false;
            do {
                token = rule[counter];
                has_epslon = false;
                // Check if the terminal already exists, if not calculate it.
                if (first.count(token) == 0 && token.compare(nonterm) != 0
                    && token.compare(prev_nonterm) != 0)
                    calc_first(token, nonterm);
                // Check for repeatence.
                if (contain_epslon[token]) has_epslon = true;
                for (string str1 : first[token]) {
                    bool found = false;
                    for (string str2 : firsts)
                        if(str1.compare(str2) == 0) {
                            found = true;
                            break;
                        }
                    if (!found) firsts.push_back(str1);
                }
                counter++;
            } while (has_epslon && rule.size() > counter);
            if (has_epslon) contain_epslon[nonterm] = true;
        }
    }
    first[nonterm].assign(firsts.begin(), firsts.end());
}

void calc_firsts() {
    for (int i = nonter.size() - 1; i >= 0; i--)
        if (first.count(nonter[i]) == 0) calc_first(nonter[i], DUMMY);
}

/* ------------------BEGIN FOLLOW CODE---------------------*/

/* Method is used to check whether a string'str' exists in vector'v',if exists
   it return its index, if not it return -1. */
int IsexistInVector (vector<string> v, string str) {
    vector<string>:: iterator pos = find(v.begin(), v.end(), str);
    unsigned int index = distance(v.begin(), pos);
    if (index < v.size()) return index;
    return -1;
}

/* Method is used to check whether a string'str1' comes after string'str2',
   if yes return 1, if no return 0. */
int compare (string str1, string str2) {
    if (IsexistInVector(nonter, str1) - IsexistInVector(nonter, str2) > 0)
        return 1;
    return 0;
}

/* Method is used to check whether the first of non-terminal's' contains
   epsilon or not. */
bool first_epsilon (string s) {
    return contain_epslon[s];
}

bool DFSCheck (vector<string> v, string str) {
    if (IsexistInVector(v, str) != -1) return true;
    for (unsigned int i = 0; i < v.size(); i++)
        if (DFSCheck((productionFollowers[v[i]].followOf), str)) return true;
    return false;
}

/// Main algorithm first iteration.
void generateInitTable () {
    for (map<string, vector<vector<string>>>::iterator it = nonterminal.begin();
                                               it != nonterminal.end(); ++it) {
        // Vector that contains the productions for a Non-Terminal.
        vector<vector<string>> productions = it->second;
        // Then, we loop on each production one by one.
        for (unsigned int index = 0; index < productions.size(); index++) {
            vector<string> proElem = productions[index];
            /* Then we loop on each production elements to find Non-Terminal
               followers. */
            for (unsigned int i = 0; i < proElem.size(); i++) {
                if (terminal.find(proElem[i]) == terminal.end()
                    && proElem[i].compare(EPSILON) != 0) {    // Non-Terminal.
                    int offset = 1;
                    /* While the following is NonTerminal, its first contains
                    epsilon and add this Non-Terminal to list of firstOf. */
                    while ((i + offset) < proElem.size()
                        && terminal.find(proElem[i + offset]) == terminal.end()
                        && first_epsilon(proElem[i + offset])) {
                        if (IsexistInVector((productionFollowers[proElem[i]]
                                       .firstOf), proElem[i + offset]) == -1) {
                            (productionFollowers[proElem[i]].firstOf)
                            .push_back(proElem[i + offset]);
                        }
                        offset ++;
                    }
                    if ((i + offset) == proElem.size()) {
                        /* Then, the last eleproElemment in the production is
                           Non-Terminal and its first is epsilon. */
                        if (DFSCheck((productionFollowers[it->first].followOf)
                                                               , proElem[i])) {
                            if (compare(proElem[i], it->first)) {
                                (productionFollowers[it->first].followOf)
                                .erase(find((productionFollowers[it->first]
                                .followOf).begin(), (productionFollowers[it->first]
                                .followOf).end(), proElem[i]));
                                (productionFollowers[proElem[i]].followOf)
                                .push_back(it->first);
                            }
                        } else if (!(proElem[i].compare(it->first) == 0 ||
                            IsexistInVector((productionFollowers[proElem[i]]
                            .followOf), proElem[i]) != -1 )) {
                            if (IsexistInVector((productionFollowers[proElem[i]]
                            .followOf), it->first) == -1) {
                                (productionFollowers[proElem[i]].followOf)
                                .push_back(it->first);
                            }
                        }
                    } else if (terminal.find(proElem[i + offset]) == terminal.end()
                        && IsexistInVector((productionFollowers[proElem[i]]
                           .firstOf), proElem[i + offset]) == -1) {
                        /* Then, it is a Non-Terminal and its first doesnot
                           contain epsilon. */
                        (productionFollowers[proElem[i]].firstOf)
                        .push_back(proElem[i + offset]);
                    } else {
                        /* Then, it is a terminal. */
                        if (IsexistInVector((productionFollowers[proElem[i]]
                            .terminalFollowers), proElem[i + offset]) == -1) {
                            (productionFollowers[proElem[i]].terminalFollowers)
                            .push_back(proElem[i + offset]);
                        }
                    }
                }
            }
        }
    }
    // Replace $ in start Non-Terminal.
    ((productionFollowers[nonter[0]]).terminalFollowers)
    .push_back(END_OF_INPUT);
}

/// Main algorithm following iterations.
void generateFollow () {
    generateInitTable();
    int done = 0;
    while (!done) {
        done = 1;
        for (map<string,followGp>::iterator it = productionFollowers.begin();
                                     it != productionFollowers.end(); ++it) {
            for (vector<string>::iterator itvf = ((it->second).followOf).begin();
                               itvf != ((it->second).followOf).end(); ++itvf) {
                if (productionFollowers[*itvf].followCalculated) {
                    for (vector<string>::iterator i = follow[*itvf].begin();
                                               i != follow[*itvf].end(); ++i) {
                        if (IsexistInVector(follow[it->first], *i) == -1)
                            follow[it->first].push_back(*i);
                    }
                    ((it -> second).followOf).erase(remove(((it -> second)
                    .followOf).begin(), ((it -> second).followOf).end(), *itvf)
                    , ((it -> second).followOf).end());
                    itvf --;
                }
            }
            for (vector<string>::iterator itvff = ((it->second).firstOf).begin();
                              itvff != ((it->second).firstOf).end(); ++itvff) {
                for (vector<string>::iterator i = first[*itvff].begin();
                                              i != first[*itvff].end(); ++i)
                    if (IsexistInVector(follow[it->first], *i) == -1)
                        follow[it->first].push_back(*i);
                ((it -> second).firstOf).erase(remove(((it -> second).firstOf)
                    .begin(), ((it -> second).firstOf).end(), *itvff) ,
                    ((it -> second).firstOf).end());
                itvff --;
            }
            for (vector<string>::iterator itvft = ((it->second).terminalFollowers)
                .begin(); itvft != ((it->second).terminalFollowers).end();
                ++itvft) {
                if (IsexistInVector(follow[it->first], *itvft) == -1)
                    follow[it->first].push_back(*itvft);
                ((it -> second).terminalFollowers).erase(remove(((it -> second)
                    .terminalFollowers).begin(), ((it -> second)
                    .terminalFollowers).end(), *itvft), ((it -> second)
                    .terminalFollowers).end());
                itvft --;
            }
            if ((((it->second).firstOf).size()) == 0 &&
                (((it->second).followOf).size()) == 0 &&
                (((it->second).terminalFollowers).size()) == 0) {
                (it->second).followCalculated = 1;
            }
            done &= (it->second).followCalculated;
        }
    }
}

vector<string> get_first (vector<string> production_rule,
                          string the_nonterminal) {
    int index = 0;
    vector<string> firsts;
    if (terminal.count(production_rule[index]) != 0)
        firsts.push_back(production_rule[index]);
    else if (production_rule[index].compare(EPSILON) == 0)
        return follow[the_nonterminal];
    else return first[production_rule[index]];
    return firsts;
}

void construct_table() {
    calc_firsts();
    generateFollow();
    vector<string> firsts;
    vector<string> synch;
    synch.push_back(SYNCH);
    for (unsigned int i = 0; i < nonter.size(); i++) {
        for (vector<string> rule : nonterminal[nonter[i]]) {
            // Add SYNCH in table.
            if (!contain_epslon[nonter[i]])
                for (string str : follow[nonter[i]])
                    if (table.count(make_pair(nonter[i], str)) == 0)
                        table[make_pair(nonter[i], str)].assign(synch.begin(),
                            synch.end());
            // Add productions in table.
            firsts = get_first(rule, nonter[i]);
            for (string str : firsts)
                table[make_pair(nonter[i], str)].assign(rule.begin(),
                    rule.end());
        }
    }
}
