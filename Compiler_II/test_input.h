int last_acceptance_state = 0;
ofstream out_file ("lexical_result.txt");
ifstream in_file ("inputFile.txt");
bool yes_or_no = true;           /* Input belongs to the grammar or not. */
bool eof = false;
vector<string> SYM_TABLE;        /* Symbol table of the parser (next phase). */
vector<char> token;              /* Keep track the token while reading from
                                    input file. */

/// Read from input file.
char read_input_file ()
{
    char result;
    if (in_file.is_open()) {
        in_file.get(result);
        if (in_file.eof()) {
            in_file.close();
            eof = true;
        }
    }
    return result;
}

/// Get next state with respect to the input.
int get_state_input (string in, int current)
{
    vector<pair<int, string>> curr = dfa_table[current];
    for (vector<pair<int, string>>::iterator it = curr.begin() + 1;
                                                        it != curr.end(); it++)
        if (it->second == in) return it->first;
    return -1;
}

/// Update the output file with tokens.
int update (string in)
{
    if (dfa_table[last_acceptance_state].front().second == "") { // Undefined token.
        out_file << "UNDEFINED_PATTERN" << endl;
        yes_or_no = false;
    } else  // Token belongs to the grammar.
        out_file << dfa_table[last_acceptance_state].front().second << endl;
    return 0;
}

/// State if input file belongs to the grammar or not.
void end_of_loop () {
    /* Write to the output file. */
    out_file << "--------------------------------------------------------------------------"
             << endl;
    out_file << "this input file " << (yes_or_no ? "belongs " :
                "does not belong ") << "to given grammar.";
    out_file.close();
}

/// Main method to read from input file and specify tokens.
string get_token ()
{
    int current = 0;
    unsigned int cnt = 0;
    int prev_cnt = 0;
    int eof_cnt = 1;
    bool file_or_token = token.empty(); /* If true -> read from file.
                                           else -> read from token. */
    char c = (file_or_token) ? read_input_file() : token[0];
    last_acceptance_state = 0;
    if (eof) return "";
    while (!eof || eof_cnt) { // Iterate on input.
        if (eof) eof_cnt--;
        else cnt++;
        string in(1, c);
        if (c == ' ' || c == '\n' || c == '\t' || eof) { // Handle whitespaces.
            if (!token.empty()) {
                current = update(in);
                // Add to Symbol table if id token.
                if (dfa_table[last_acceptance_state].front().second == "id")
                    SYM_TABLE.push_back(string(token.begin(),
                              token.begin() + ((prev_cnt) ? prev_cnt : cnt)));
                token.erase(token.begin(), token.begin() + cnt - 1);
                if (dfa_table[last_acceptance_state].front().second == "") {
                    cnt = 0;
                    if (!file_or_token && token.size() > cnt)
                        file_or_token = token.empty();
                    else file_or_token = true;
                    if (eof) eof_cnt--;
                    continue;
                }
                break;
            }
            cnt--;
        } else { // Get next state.
            if (file_or_token) token.push_back(c);
            int next_state = get_state_input(in, current);
            if (next_state == -1) {
                // Unaccepted state then handle previous token (if exist).
                current = update(in);
                // Add to Symbol table if id token.
                if (dfa_table[last_acceptance_state].front().second == "id")
                    SYM_TABLE.push_back(string(token.begin(),
                              token.begin() + ((prev_cnt) ? prev_cnt : cnt)));
                token.erase(token.begin(), token.begin() + ((prev_cnt) ?
                                                             prev_cnt : cnt));
                if (dfa_table[last_acceptance_state].front().second == "") {
                    cnt = 0;
                    if (!file_or_token && token.size() > cnt)
                        file_or_token = token.empty();
                    else file_or_token = true;
                    if (eof) eof_cnt--;
                    continue;
                }
                break;
            }
            // Accepted state then set current to complete.
            current = next_state;
            if (dfa_table[current].front().second != "") {
                last_acceptance_state = current;
                prev_cnt = cnt;
            }
        }
        if (!file_or_token && token.size() > cnt)
            file_or_token = token.empty();
        else file_or_token = true;
        c = (file_or_token) ? read_input_file() : token[cnt];
    }
    if (eof) {
        end_of_loop();
    }
    /* With assumption that if no more tokens then parser would get an empty
       string so that it can stop. */
    return last_acceptance_state == 0 ? "" :
           dfa_table[last_acceptance_state].front().second;
}
