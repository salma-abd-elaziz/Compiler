ofstream parse_file ("parser_result.txt");

/// Handle indentation of printing the table into the output file.
void print_table () {
    terminal["$"] = "";
    map<string, string> mapping;
    unsigned int non_terminal_size = 8;
    unsigned int terminal_size = 8;
    unsigned int sz[terminal.size()];
    char c = 'A';
    // Calculate the maximum size for non-terminal column.
    for (vector<string>::iterator it = nonter.begin(); it != nonter.end();
                                                                          it++)
        if ((*it).size() > non_terminal_size) non_terminal_size = (*it).size();
    // Calculate the maximum size for terminal column.
    for (map<string, string>::iterator it = terminal.begin();
                                       it != terminal.end(); it++)
        if ((it->first).size() > terminal_size)
            terminal_size = (it->first).size();
    cout << GREEN << "\t\t\tPredective Parsing Table" << endl;
    cout << "\t\t\t------------------------" << RESET << endl;
    parse_file << "\t\t\tPredective Parsing Table" << endl;
    parse_file << "\t\t\t------------------------" << endl;
    // Print mapping.
    cout << CYAN << "\t** Mapping :" << endl;
    cout << "\t   ---------" << RESET << endl;
    cout << "--------";
    parse_file << "\t** Mapping :" << endl;
    parse_file << "\t   ---------" << endl;
    parse_file << "--------";
    for (unsigned int i = 0; i <= non_terminal_size; i++) {
        cout << "-";
        parse_file << "-";
    }
    cout << setw(4) << "";
    cout << "--------";
    parse_file << setw(4) << "";
    parse_file << "--------";
    for (unsigned int i = 0; i <= terminal_size; i++) {
        cout << "-";
        parse_file << "-";
    }
    cout << endl;
    cout << setw(8) << left << "Name";
    cout << setw(non_terminal_size + 1) << left << "original";
    cout << setw(4) << "";
    cout << setw(8) << left << "Name";
    cout << setw(terminal_size) << left << "original" << endl;
    cout << "--------";
    parse_file << endl;
    parse_file << setw(8) << left << "Name";
    parse_file << setw(non_terminal_size + 1) << left << "original";
    parse_file << setw(4) << "";
    parse_file << setw(8) << left << "Name";
    parse_file << setw(terminal_size) << left << "original" << endl;
    parse_file << "--------";
    for (unsigned int i = 0; i <= non_terminal_size; i++) {
        cout << "-";
        parse_file << "-";
    }
    cout << setw(4) << "";
    cout << "--------";
    parse_file << setw(4) << "";
    parse_file << "--------";
    for (unsigned int i = 0; i <= terminal_size; i++) {
        cout << "-";
        parse_file << "-";
    }
    cout << endl;
    parse_file << endl;
    map<string, string>::iterator ter = terminal.begin();
    char t = 'a';
    for (vector<string>::iterator it = nonter.begin(); it != nonter.end();
                                                       it++) {
        mapping[*it] = string(1, c);
        cout << setw(4) << left << c;
        cout << "\t";
        cout << setw(non_terminal_size + 1) << left << *it;
        parse_file << setw(4) << left << c;
        parse_file << "\t";
        parse_file << setw(non_terminal_size + 1) << left << *it;
        c++;
        cout << setw(4) << "";
        parse_file << setw(4) << "";
        if (ter != terminal.end()) {
            mapping[ter->first] = string(1, t);
            cout << setw(4) << left << t;
            cout << setw(4) << "";
            cout << setw(terminal_size) << left << ter->first << endl;
            parse_file << setw(4) << left << t;
            parse_file << setw(4) << "";
            parse_file << setw(terminal_size) << left << ter->first << endl;
            t++;
            ter++;
        } else {
            cout << endl;
            parse_file << endl;
        }
    }
    int i = 0;
    while (ter != terminal.end()) {
        if (i == 0) {
            mapping[SYNCH] = string(1, c);
            cout << setw(4) << left << c;
            cout << "\t";
            cout << setw(non_terminal_size + 1) << left << SYNCH;
            parse_file << setw(4) << left << c;
            parse_file << "\t";
            parse_file << setw(non_terminal_size + 1) << left << SYNCH;
            c++;
            i++;
            cout << setw(4) << "";
            parse_file << setw(4) << "";
        } else if (i == 1) {
            mapping[EPSILON] = string(1, c);
            cout << setw(4) << left << c;
            cout << "\t";
            cout << setw(non_terminal_size + 1) << left << EPSILON;
            parse_file << setw(4) << left << c;
            parse_file << "\t";
            parse_file << setw(non_terminal_size + 1) << left << EPSILON;
            i++;
            cout << setw(4) << "";
            parse_file << setw(4) << "";
        } else {
            cout << setw(13 + non_terminal_size) << "";
            parse_file << setw(13 + non_terminal_size) << "";
        }
        mapping[ter->first] = string(1, t);
        cout << setw(4) << left << t;
        cout << setw(4) << "";
        cout << setw(terminal_size) << left << ter->first << endl;
        parse_file << setw(4) << left << t;
        parse_file << setw(4) << "";
        parse_file << setw(terminal_size) << left << ter->first << endl;
        t++;
        ter++;
    }
    cout << "--------";
    parse_file << "--------";
    for (unsigned int i = 0; i <= non_terminal_size; i++) {
        cout << "-";
        parse_file << "-";
    }
    cout << setw(4) << "";
    cout << "--------";
    parse_file << setw(4) << "";
    parse_file << "--------";
    for (unsigned int i = 0; i <= terminal_size; i++) {
        cout << "-";
        parse_file << "-";
    }
    cout << endl;
    parse_file << endl;
    // Print table with mapping.
    cout << CYAN << "\t** Table :" << endl;
    cout << "\t   -------" << RESET << endl;
    parse_file << "\t** Table :" << endl;
    parse_file << "\t   -------" << endl;
    unsigned int index = 0;
    // Calculate maximum lengths.
    int line = 3;
    for (map<string, string>::iterator it = terminal.begin();
                                       it != terminal.end(); it++) {
        sz[index] = (it->first).size();
        for (vector<string>::iterator non_ter = nonter.begin();
                                      non_ter != nonter.end(); non_ter++)
            if (table.count(make_pair(*non_ter, it->first))) {
                vector<string> v = table[make_pair(*non_ter, it->first)];
                if (v.size() > sz[index]) sz[index] = v.size();
            }
        line += sz[index] + 3;
        index++;
    }
    // Print body.
    for (int i = 0; i < line; i++) {
        cout << "-";
        parse_file << "-";
    }
    cout << endl;
    cout << setw(1) << "";
    cout << "  ";
    parse_file << endl;
    parse_file << setw(1) << "";
    parse_file << "  ";
    index = 0;
    // Print terminal header in table.
    for (map<string, string>::iterator it = terminal.begin();
                                       it != terminal.end(); it++) {
        cout << setw(sz[index]) << left << mapping[it->first];
        cout << setw(3) << "";
        parse_file << setw(sz[index]) << left << mapping[it->first];
        parse_file << setw(3) << "";
        index++;
    }
    cout << endl;
    parse_file << endl;
    for (int i = 0; i < line; i++) {
        cout << "-";
        parse_file << "-";
    }
    cout << endl;
    parse_file << endl;
    for (vector<string>::iterator non_ter = nonter.begin();
                                  non_ter != nonter.end(); non_ter++) {
        cout << setw(3) << left << mapping[*non_ter];
        parse_file << setw(3) << left << mapping[*non_ter];
        index = 0;
        for (map<string, string>::iterator it = terminal.begin();
                                           it != terminal.end(); it++) {
            if (table.count(make_pair(*non_ter, it->first))) {
                string s = "";
                for (string l : table[make_pair(*non_ter, it->first)])
                    s.append(mapping[l]);
                cout << setw(sz[index]) << left << s.substr(0, s.size());
                cout << setw(3) << "";
                parse_file << setw(sz[index]) << left << s.substr(0, s.size());
                parse_file << setw(3) << "";
            } else {
                cout << setw(sz[index]) << "";
                cout << setw(3) << "";
                parse_file << setw(sz[index]) << "";
                parse_file << setw(3) << "";
            }
            index++;
        }
        cout << endl;
        parse_file << endl;
    }
    for (int i = 0; i < line; i++) {
        cout << "-";
        parse_file << "-";
    }
    cout << endl;
    parse_file << endl;
}

/// Handle parsing and specify suitable errors for each non-matching case.
void parse () {
    cout << endl;
    cout << GREEN << "\t\t\tLeftmost Derivation" << endl;
    cout << "\t\t\t-------------------" << RESET << endl;
    parse_file << endl;
    parse_file << "\t\t\tLeftmost Derivation" << endl;
    parse_file << "\t\t\t-------------------" << endl;
    // Stack of the parser to keep track the left most drivation.
    stack<string> track;
    // Enter start symbol (nonterminal).
    track.push(nonter.front());
    bool flag = true;
    string token = get_token();
    while (flag) {
        if (!token.empty() && !track.empty()) { // Continue parsing.
            if (terminal.count(track.top())) { // Match stack with input.
                if (token == track.top()) {
                    track.pop();
                    token = get_token();
                } else { // Specify suitable errors.
                    if (track.size() > 1) {
                        cout << RED << "Missing '" << track.top() <<
                                       "', inserted." << RESET << endl;
                        parse_file << track.top() << "', inserted." << endl;
                        track.pop();
                        continue;
                    } else {
                        cout << RED << "Extra '" << token << "', discarded."
                             << RESET << endl;
                        parse_file << "Extra '" << token << "', discarded."
                                   << endl;
                    }
                }
            } else { // Replace nonterminal with its production in stack.
                vector<string> pro = table[make_pair(track.top(), token)];
                if (pro.empty()) { // Error.
                    cout << RED << "Extra '" << token << "', discarded."
                         << RESET << endl;
                    parse_file << "Extra '" << token << "', discarded."
                               << endl;
                    token = get_token();
                } else if (pro[0] == EPSILON) { // This token is in the
                                                // followers (go to epsilon).
                    cout << CYAN << track.top() << " -> " << RESET << EPSILON
                         << endl;
                    parse_file << track.top() << " -> " << EPSILON << endl;
                    track.pop();
                } else if (pro[0] == SYNCH) { // This token in the followers
                                              // (error).
                    cout << RED << "Illegal '" << track.top() <<
                                   "', discarded." << RESET << endl;
                    parse_file << "Illegal '" << track.top() << "', discarded."
                               << endl;
                    track.pop();
                } else { // It is a production.
                    cout << CYAN << track.top() << " -> " << RESET;
                    parse_file << track.top() << " -> ";
                    for (vector<string>::iterator it = pro.begin();
                                                  it != pro.end(); it++) {
                        cout << *it << " ";
                        parse_file << *it << " ";
                    }
                    cout << endl;
                    parse_file << endl;
                    track.pop();
                    for (vector<string>::iterator it = pro.end() - 1;
                                                  it >= pro.begin(); it--)
                        track.push(*it);
                }
            }
        } else { // Errors.
            if (token.empty()) {
                while (!track.empty()) {
                    if (terminal.count(track.top())) {
                        cout << RED << "Missing '" << track.top() <<
                                       "', inserted." << RESET << endl;
                        parse_file << "Missing '" << track.top() <<
                                      "', inserted." << endl;
                    }
                    else if (table.count(make_pair(track.top(), END_OF_INPUT))
                             && (table[make_pair(track.top(),
                                               END_OF_INPUT)])[0] == EPSILON) {
                        cout << CYAN << track.top() << " -> " << RESET
                             << EPSILON << endl;
                        parse_file << track.top() << " -> " << EPSILON << endl;
                    }
                    else {
                        cout << RED << "Illegal '" << track.top() <<
                                       "', discarded." << RESET << endl;
                        parse_file << "Illegal '" << track.top() <<
                                      "', discarded." << endl;
                    }
                    track.pop();
                }
            } else {
                while (!token.empty()) {
                    cout << RED << "Extra '" << token << "', discarded."
                         << RESET << endl;
                    parse_file << "Extra '" << token << "', discarded."
                               << endl;
                    token = get_token();
                }
            }
            flag = false;
        }
    }
    cout << GREEN << "Parsing is done." << RESET << endl;
    parse_file << "Parsing is done." << endl;
    parse_file.close();
}
