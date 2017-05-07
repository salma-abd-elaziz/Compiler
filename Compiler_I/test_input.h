#define RESET "\033[0m"
#define RED "\033[1m\033[31m"

int last_acceptance_state;
ofstream out_file ("result.txt");
bool yes_or_no = true;              /* Input belongs to the grammar or not. */
vector<string> SYM_TABLE;           /* Symbol table of the parser (next phase). */

/// Read input file.
string read_input_file (string file_name)
{
    string line;
    string result = "";
    ifstream myfile (file_name.c_str());
    if (myfile.is_open()) {
        while (getline(myfile, line))
            result += line + "\n";
        myfile.close();
    }
    else cout << "Unable to open file";
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
        cout << RED <<"UNDEFINED_PATTERN " << RESET;
        out_file << "UNDEFINED_PATTERN" << endl;
        yes_or_no = false;
    } else { // Token belongs to the grammar.
        cout << dfa_table[last_acceptance_state].front().second
                                 << ((in == "\n") ? "\n" : " ");
        out_file << dfa_table[last_acceptance_state].front().second << endl;
    }
    return 0;
}

/// Handle white spaces and new lines.
string handle_input (string in)
{
    /* Replace tabs with single space. */
    size_t space = in.find("\t");
    while (space < in.size()) {
        in.replace(space, 1, " ");
        space = in.find("\t");
    }
    /* Handle new lines. */
    space = in.find(" \n");
    while (space < in.size()) {
        in.replace(space, 2, "\n");
        space = in.find(" \n");
    }
    /* Handle multiple space. */
    space = in.find("  ");
    while (space < in.size()) {
        in.replace(space, 2, " ");
        space = in.find("  ");
    }
    return in;
}

void get_output ()
{
    int current = 0;
    int cnt = 0;
    int prev_cnt;
    string input = handle_input(read_input_file("inputFile.txt").c_str());
    string::iterator prev_it;
    for (string::iterator it = input.begin(); input.size() != 0
         && it != input.end(); it++) { // Iterate on input.
        cnt++;
        string in = "";
        in += *it;
        if (in == "\n" || in == " " || in == "\t") { // Handle the token previous to space or new line.
            if (it + 1 == input.end()
                && dfa_table[last_acceptance_state].front().second == "")
                continue;
            current = update(in);
            if (dfa_table[last_acceptance_state].front().second == "id") // Add to Symbol table if id token.
                SYM_TABLE.push_back(input.substr(0, cnt - 1));
            // Reset.
            last_acceptance_state = 0;
            input.replace(0, cnt, "");
            while (*input.begin() == ' ' || *input.begin() == '\n')
                input.replace(0, 1, "");
            it = input.begin() - 1;
            cnt = 0;
            prev_cnt = 0;
        }
        else { // Get next state.
            int next_state = get_state_input(in, current);
            if (next_state == -1) { // Unaccepted state then handle previous token (if exist).
                current = update(in);
                if (dfa_table[last_acceptance_state].front().second == "id") // Add to Symbol table if id token.
                    SYM_TABLE.push_back(input
                                    .substr(0, (prev_cnt) ? prev_cnt : cnt));
                //Reset
                last_acceptance_state = 0;
                input.replace(0, (prev_cnt) ? prev_cnt : cnt, "");
                while (*input.begin() == ' ' || *input.begin() == '\n')
                    input.replace(0, 1, "");
                it = input.begin() - 1;
                cnt = 0;
                prev_cnt = 0;
                continue;
            }
            // Accepted state then set current to complete.
            current = next_state;
            if (dfa_table[current].front().second != "") {
                last_acceptance_state = current;
                prev_cnt = cnt;
            }
        }
    }
    /* Write to the output file. */
    cout << "\n--------------------------------------------------------------------------"
         << endl;
    cout << "this input file "
         << (yes_or_no ? "\033[1m\033[32mbelongs " : "\033[1m\033[31mdoes not belong ")
         << RESET << "to given grammar.";
    out_file << "--------------------------------------------------------------------------"
             << endl;
    out_file << "this input file " << (yes_or_no ? "belongs " : "does not belong ")
             << "to given grammar.";
    out_file.close();
}
