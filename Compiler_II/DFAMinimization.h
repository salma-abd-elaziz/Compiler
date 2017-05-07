#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

map<int, vector<pair<int, string>>> dfa_table;

void initializeNonterminals (map<string, vector<int>> &accStates,
                             vector<int> &v_min, map<int, int> &partitions)
{
    /* Step 1 : partition the terminal states using hash map (accStates) and
       insert all non-terminal states as one partition in vector (v_min). */
    for (map <int, vector<pair<int, string>>>::iterator it = dfa_table.begin();
                                                 it != dfa_table.end(); ++it) {
         pair<int, string> p = (it->second)[0];
         if (p.second != "") { // It is a terminal state.
            if (accStates[p.second].empty())
                vector <int> v_groupAcc;
            accStates[p.second].push_back(it->first);
         } else {
            v_min.push_back(it->first);
            partitions[it->first] = 0;
         }
    }
    // Add a num (-1) after the non terminal partition in the vector (v_min).
    v_min.push_back(-1);
}

void initializeTerminals (map<string, vector<int>> &accStates,
                          vector<int> &v_min, map<int, int> &partitions)
{
    /* Step 2 : add the terminal partitions into the vector (v_min). */
    int partitionNum = 1;
    for (map<string, vector<int>>::iterator it = accStates.begin();
                                        it != accStates.end(); ++it) {
        for (unsigned int j = 0; j < (it->second).size(); j++) {
            v_min.push_back((it->second)[j]);
            partitions[(it->second)[j]] = partitionNum;
        }
        v_min.push_back(-1);
        partitionNum++;
    }
}

void reconstructMinimumDFA (vector<int> &v_min)
{
    // Rewrite the minimized dfa_table.
    map<int, int> replacement;
    int representive = 0;
    signed int size = v_min.size();
    for (int i = 0; i < size; i++) {
        if (!(v_min[i - 1] == -1 || i - 1 < 0)) {
            replacement[v_min[i]] = representive;
            dfa_table.erase(v_min[i]);
        } else {
            representive = v_min[i];
        }
    }
    for (map<int, vector<pair<int, string>>>::iterator it = dfa_table.begin();
                                                 it != dfa_table.end(); ++it) {
        for (unsigned int i = 0; i < (it->second).size(); i++) {
            if ((it->second)[i].first != -1) {
                map<int, int>::iterator it2 = replacement.find((it->second)[i]
                                                                       .first);
                if (it2 != replacement.end())
                    (it->second)[i].first = replacement[(it->second)[i].first];
            }
        }
    }
}

void partition (vector<int> &v_min, map<int, int> &partitions)
{
    map<int, vector<int>> previousTemp;
    map<int, vector<int>> temp;
    map<pair<string, int>, vector<int>> groupingMap;
    int stop = 0;
    while (!stop) {
        unsigned int i = 0;
        while (i < v_min.size()) { // All partitions.
            for (int k = i; v_min[k] != -1; k++) { // States in a partition.
                string strAllInputs = "";
                for (unsigned int j = 1; j < (dfa_table[v_min[k]]).size();
                     j++) { // Inputs.
                    if (((dfa_table[v_min[k]])[j]).first == -1) {
                        string str = SSTR(-1);
                        strAllInputs += str;
                    } else {
                        int num_partition =
                                partitions[((dfa_table[v_min[k]])[j]).first];
                        string str = SSTR(num_partition);
                        strAllInputs += str;
                    }
                }
                pair<string, int> p (strAllInputs, partitions[v_min[k]]);
                if (groupingMap[p].empty())
                    vector<int> statesOfPar;
                groupingMap[p].push_back(v_min[k]);
                i = k;
            }
            i = i + 2;
        }
        // Refill the v_min vector.
        int partition_num = 0;
        v_min.clear();
        for (map<pair<string, int>, vector<int>>::iterator it = groupingMap
                                     .begin(); it != groupingMap.end(); ++it) {
            for (unsigned int a = 0; a < (it->second).size(); a++) {
                v_min.push_back((it->second)[a]);
                if (temp[partition_num].empty()) vector<int> Par;
                temp[partition_num].push_back((it->second)[a]);
                partitions[(it->second)[a]] = partition_num;
            }
            v_min.push_back(-1);
            partition_num++;
        }
        groupingMap.clear();
        unsigned int counter = 0;
        if (!previousTemp.empty() && previousTemp.size() == temp.size())
            for (unsigned int f = 0; f < temp.size(); f++)
                for (unsigned int ff = 0; ff < previousTemp.size(); ff++)
                    if (temp[f] == previousTemp[ff]) {
                        counter++;
                        break;
                    }
        if (counter == temp.size()) stop = 1;
        previousTemp.clear();
        previousTemp = temp;
        temp.clear();
    }
    reconstructMinimumDFA(v_min);
}

void initialize_partitions (map<string, vector<int>> &accStates,
                            vector<int> &v_min, map<int, int> &partitions)
{
    initializeNonterminals(accStates, v_min, partitions);
    initializeTerminals(accStates, v_min, partitions);
}

// Public method which take the unminimized dfa map pointer minimize it.
void generate_automata ()
{
    SubsetConstruction b;
    b.get_DFA();
    swap(b.DFA, dfa_table);
    map<string, vector<int>> accStates;
    map<int, int> partitions;
    vector<int> v_min;
    initialize_partitions (accStates, v_min, partitions);
    // The main algorithim "partitioning".
    partition(v_min, partitions);
}
