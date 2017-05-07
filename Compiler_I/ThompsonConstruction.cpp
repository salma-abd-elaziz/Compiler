#include "ThompsonConstruction.h"

ThompsonConstruction::ThompsonConstruction ()
{
    letter.assign("([a-zA-Z]+)\\s*=\\s*(.)[-](.)\\s*[|]\\s*(.)[-](.)");
    digit.assign("([a-zA-Z]+)\\s*=\\s*(.)\\s*[-]\\s*(.)\\s*");
    /* Regular expressions. */
    id.assign("([a-zA-Z]+)\\s*:\\s*([a-zA-Z]+)\\s*[(]\\s*([a-zA-Z]+)\\s*[|]\\s*([a-zA-Z]+)\\s*[)][*]\\s*");
    digits.assign("([a-zA-Z]+)\\s*=\\s*([a-zA-Z]+)\\+\\s*");
    num.assign("([a-zA-Z]+)\\s*:\\s*([a-zA-Z]+)\\+\\s*[|]\\s*([a-zA-Z]+)\\+\\s*(\\.)\\s*([a-zA-Z]+)\\s*[(]\\s*\\\\(L)\\s*[|]\\s*(E)\\s*([a-zA-Z]+)[)]\\s*");
    keywords.assign("[{]\\s*([a-zA-Z]+)\\s+([a-zA-Z]+)\\s+([a-zA-Z]+)\\s*[}]");
    punctuations.assign("\\[(.)\\s(.)\\s\\\\(.)\\s\\\\(.)\\s(.)\\s(.)\\]");
    arithop.assign("([a-zA-Z]+)\\s*:\\s*\\\\(.)\\s*[|]\\s*(.)");
    assi.assign("([a-zA-Z]+)\\s*:\\s*(=)");
    relop.assign("([a-zA-Z]+)\\s*:\\s*(.+)\\s*[|]\\s*(.+)\\s*[|]\\s*(.+)\\s*[|]\\s*(.+)\\s*[|]\\s*(.+)\\s*[|]\\s*(.+)\\s*");
    /* Create start state for final NFA. */
    State start;
    start.id = 0;
    final_NFA.insert(make_pair(0,start));
}

Automata ThompsonConstruction::create_automata (range r)
{
    State st;
    State en;
    st.id = unique_id++;
    en.id = unique_id++;
    st.acceptance = "";
    en.acceptance = "";
    st.transition.push_back(make_pair(en.id,r));
    final_NFA.insert(make_pair(st.id,st));
    final_NFA.insert(make_pair(en.id,en));
    Automata result;
    result.start_state = st;
    result.end_state = en;
    return result;
}

Automata ThompsonConstruction::disjunction (Automata first, Automata second)
{
    Automata result;
    // Start.
    State st;
    st.id = unique_id++;
    st.acceptance = "";
    st.epslon_closure.push_back(first.start_state.id);
    st.epslon_closure.push_back(second.start_state.id);
    final_NFA.insert(make_pair(st.id, st));
    // End.
    State en;
    en.id = unique_id++;
    en.acceptance = "";
    first.end_state.epslon_closure.push_back(en.id);
    second.end_state.epslon_closure.push_back(en.id);
    final_NFA.insert(make_pair(en.id, en));
    final_NFA.erase(final_NFA.find(first.end_state.id));
    final_NFA.insert(make_pair(first.end_state.id, first.end_state));
    final_NFA.erase(final_NFA.find(second.end_state.id));
    final_NFA.insert(make_pair(second.end_state.id, second.end_state));
    // Result.
    result.start_state = st;
    result.end_state = en;
    return result;
}

Automata ThompsonConstruction::disjunction_more (vector<Automata> automatas)
{
    Automata result;
    /* Create start and end states. */
    State st,en;
    st.id = unique_id++;
    en.id = unique_id++;
    st.acceptance = "";
    en.acceptance = "";
    /* Combine. */
    for(Automata aut : automatas) {
        st.epslon_closure.push_back(aut.start_state.id);
        aut.end_state.epslon_closure.push_back(en.id);
        final_NFA.erase(final_NFA.find(aut.end_state.id));
        final_NFA.insert(make_pair(aut.end_state.id, aut.end_state));
    }
    /* Insert new states in final NFA states. */
    final_NFA.insert(make_pair(st.id,st));
    final_NFA.insert(make_pair(en.id,en));
    /* Set result. */
    result.start_state = st;
    result.end_state = en;
    return result;
}

Automata ThompsonConstruction::concatenation (Automata first, Automata second)
{
    Automata result;
    /* Create new automata. */
    // Move all transition states of second automata to first automata.
    for(pair<int,range> state : second.start_state.transition)
        first.end_state.transition.push_back(make_pair(state.first,
                                                                state.second));
    for(int state : second.start_state.epslon_closure)
        first.end_state.epslon_closure.push_back(state);
    final_NFA.erase(final_NFA.find(second.start_state.id));
    final_NFA.erase(final_NFA.find(first.end_state.id));
    final_NFA.insert(make_pair(first.end_state.id, first.end_state));
    // Result.
    result.start_state = first.start_state;
    result.end_state = second.end_state;
    return result;
}

Automata ThompsonConstruction::closure (Automata base)
{
    Automata result;
    /* Create new automata. */
    State st;
    State en;
    st.id = unique_id++;
    en.id = unique_id++;
    st.acceptance = "";
    en.acceptance = "";
    st.epslon_closure.push_back(base.start_state.id);
    st.epslon_closure.push_back(en.id);
    base.end_state.epslon_closure.push_back(en.id);
    base.end_state.epslon_closure.push_back(base.start_state.id);
    final_NFA.erase(final_NFA.find(base.end_state.id));
    final_NFA.insert(make_pair(base.end_state.id, base.end_state));
    final_NFA.insert(make_pair(st.id, st));
    final_NFA.insert(make_pair(en.id, en));
    // Result.
    result.start_state = st;
    result.end_state = en;
    return result;
}

void ThompsonConstruction::read_file ()
{
    string line;
    ifstream myfile("input.txt");
    if (myfile.is_open()) {
        while (getline(myfile, line)) {
            /* Match line with one of regular definitions. */
            if (std::regex_search(line, m, letter)) {
                string token = m[1];
                string start_rang1 = m[2];
                string end_rang1 = m[3];
                string start_rang2 = m[4];
                string end_rang2 = m[5];
                range ran1;
                ran1.start = start_rang1.at(0);
                ran1.end_ = end_rang1.at(0);
                range ran2;
                ran2.start = start_rang2.at(0);
                ran2.end_ = end_rang2.at(0);
                vector<range> ranges;
                ranges.push_back(ran1);
                ranges.push_back(ran2);
                // Push in basic alphabet.
                basic_alph.insert(ran1);
                basic_alph.insert(ran2);
                alphabet.insert(make_pair(token, ranges));
                Automata first = create_automata(ran1);
                Automata second = create_automata(ran2);
                Automata result = disjunction(first, second);
                /* Make the last state accept this token. */
                final_NFA.find(result.end_state.id)->second.acceptance = token;
                /* Combine with final NFA. */
                final_NFA.find(0)->second.epslon_closure
                                         .push_back(result.start_state.id);
            } else if (std::regex_search(line, m, digit)) {
                string token = m[1];
                string start_rang = m[2];
                string end_rang = m[3];
                range ran;
                ran.start = start_rang.at(0);
                ran.end_ = end_rang.at(0);
                vector<range> ranges;
                ranges.push_back(ran);
                // Push in basic alphabet.
                basic_alph.insert(ran);
                alphabet.insert(make_pair(token, ranges));
                Automata result = create_automata(ran);
                /* Make the last state accept this token. */
                final_NFA.find(result.end_state.id)->second.acceptance = token;
                /* Combine with final NFA. */
                final_NFA.find(0)->second.epslon_closure
                                         .push_back(result.start_state.id);
            } else if (std::regex_search(line, m, digits)) {
                string token = m[1];
                string older_def = m[2];
                /* Get older definition range. */
                vector<range> v;
                range ran;
                std::map<string,vector<range>>::iterator it;
                it = alphabet.find(older_def);
                if (it != alphabet.end())
                    v = it->second;
                ran.start = v.at(0).start;
                ran.end_ = v.at(0).end_;
                vector<range> ranges;
                ranges.push_back(ran);
                ranges.push_back(ran);
                alphabet.insert(make_pair(token, ranges));
                Automata result = concatenation(create_automata(ran),
                                                closure(create_automata(ran)));
                /* Make the last state accept this token. */
                final_NFA.find(result.end_state.id)->second.acceptance = token;
                /* Combine with final NFA. */
                final_NFA.find(0)->second.epslon_closure
                                         .push_back(result.start_state.id);
            } else if (std::regex_search(line, m, id)) {
                string token = m[1];
                string first = m[2];
                string second = m[3];
                string third = m[4];
                Automata fir;
                Automata sec;
                Automata thi;
                vector<range> v;
                std::map<string,vector<range>>::iterator it;
                /* Get first automata. */
                it = alphabet.find(first);
                if (it != alphabet.end()) {
                    v = it->second;
                    // Disjunction ranges.
                    if ( v.size() == 2 && v.at(0).start != v.at(1).start
                                       && v.at(0).end_ != v.at(1).end_)
                        fir = disjunction(create_automata(v.at(0)),
                                          create_automata(v.at(1)));
                }
                /* Get second automata. */
                it = alphabet.find(second);
                if (it != alphabet.end()) {
                    v = it->second;
                    if (v.size() == 2 && v.at(0).start != v.at(1).start
                        && v.at(0).end_ != v.at(1).end_) // Disjunction ranges.
                        sec = disjunction(create_automata(v.at(0)),
                                          create_automata(v.at(1)));
                    else if (v.size() == 2 && v.at(0).start == v.at(1).start
                             && v.at(0).end_ == v.at(1).end_) // One or more ranges.
                        sec = concatenation(create_automata(v.at(0)),
                                            closure(create_automata(v.at(1))));
                    else if (v.size() == 1) // Single ranges.
                        sec = create_automata(v.at(0));
                }
                /* Get third automata. */
                it = alphabet.find(third);
                if (it != alphabet.end()) {
                    v = it->second;
                    if (v.size() == 2) // Disjunction ranges.
                        thi = disjunction(create_automata(v.at(0)),
                                          create_automata(v.at(1)));
                    else if (v.size() == 1) // Single range.
                        thi = create_automata(v.at(0));
                }
                Automata result = concatenation(fir, closure(disjunction(sec,
                                                                        thi)));
                /* Make the last state accept this token. */
                final_NFA.find(result.end_state.id)->second.acceptance = token;
                /* Combine with final NFA. */
                final_NFA.find(0)->second.epslon_closure
                                         .push_back(result.start_state.id);
            } else if (std::regex_search(line, m, keywords)) {
                range r;
                Automata first, second;
                for (unsigned int i = 1; i < m.size(); i++) {
                    bool begining = true;
                    string keyword = m[i];
                    for (unsigned int i = 0; i < keyword.size(); i++) {
                        r.start = keyword.at(i);
                        r.end_ = keyword.at(i);
                        if (begining) {
                            first = create_automata(r);
                            begining = false;
                        } else {
                            second = create_automata(r);
                            first = concatenation(first, second);
                        }
                    }
                    /* Make the last state accept this token. */
                    final_NFA.find(first.end_state.id)->second
                                                        .acceptance = keyword;
                    /* Combine with final NFA. */
                    final_NFA.find(0)->second.epslon_closure
                                             .push_back(first.start_state.id);
                }
            } else if (std::regex_search(line,m,punctuations)) {
                range r;
                for (unsigned int i = 1; i < m.size(); i++) {
                    string symb = m[i];
                    r.start = symb.at(0);
                    r.end_ = symb.at(0);
                    // Push in basic alphabet.
                    basic_alph.insert(r);
                    Automata result = create_automata(r);
                    /* Make the last state accept this token. */
                    final_NFA.find(result.end_state.id)->second
                                                         .acceptance = symb;
                    /* Combine with final NFA. */
                    final_NFA.find(0)->second.epslon_closure
                                             .push_back(result.start_state.id);
                }
            } else if (std::regex_search(line, m, arithop)) {
                string token = m[1];
                range r;
                /* Get first operand. */
                string symb = m[2];
                r.start = symb.at(0);
                r.end_ = symb.at(0);
                // Push in basic alphabet.
                basic_alph.insert(r);
                Automata result = create_automata(r);
                /* Get sec operand. */
                symb = m[3];
                r.start = symb.at(0);
                r.end_ = symb.at(0);
                // Push in basic alphabet.
                basic_alph.insert(r);
                result = disjunction(result, create_automata(r));
                /* Make the last state accept this token. */
                final_NFA.find(result.end_state.id)->second.acceptance = token;
                /* Combine with final NFA. */
                final_NFA.find(0)->second.epslon_closure
                                         .push_back(result.start_state.id);
            } else if (std::regex_search(line, m, num)) {
                // exp : first+ | second
                // second : frac1+ . frac2 exp
                string token = m[1];
                Automata first;
                Automata exponent;
                Automata fraction;
                /* Create the first automata. */
                std::map<string, vector<range>>::iterator it;
                vector<range> v;
                it = alphabet.find(m[2]);
                if (it != alphabet.end()) {
                    v = it->second;
                    if (v.size() == 2 && v.at(0).start != v.at(1).start
                        && v.at(0).end_ != v.at(1).end_) // Disjunction ranges.
                        first = disjunction(create_automata(v.at(0)),
                                            create_automata(v.at(1)));
                    else if (v.size() == 2 && v.at(0).start == v.at(1).start
                             && v.at(0).end_ == v.at(1).end_) // One or more ranges.
                        first = concatenation(create_automata(v.at(0)),
                                            closure(create_automata(v.at(1))));
                    else if (v.size() == 1) // Single ranges.
                        first = create_automata(v.at(0));
                }
                first = concatenation(first, closure(first));
                /* Get automata for fraction part. */
                Automata frac1;
                Automata frac2;
                it = alphabet.find(m[3]);
                if (it != alphabet.end()) {
                    v = it->second;
                    if (v.size() == 2 && v.at(0).start != v.at(1).start
                        && v.at(0).end_ != v.at(1).end_) // Disjunction ranges.
                        frac1 = disjunction(create_automata(v.at(0)),
                                                     create_automata(v.at(1)));
                    else if (v.size() == 2 && v.at(0).start == v.at(1).start
                             && v.at(0).end_ == v.at(1).end_) // One or more ranges.
                        frac1 = concatenation(create_automata(v.at(0)),
                                            closure(create_automata(v.at(1))));
                    else if (v.size() == 1) // Single ranges.
                        frac1 = create_automata(v.at(0));
                }
                frac1 = concatenation(frac1, closure(frac1));
                string frac_symbol = m[4];
                range r;
                r.start = frac_symbol.at(0);
                r.end_ = frac_symbol.at(0);
                frac1 = concatenation(frac1, create_automata(r));
                it = alphabet.find(m[5]);
                if (it != alphabet.end()) {
                    v = it->second;
                    if (v.size() == 2 && v.at(0).start != v.at(1).start
                        && v.at(0).end_ != v.at(1).end_) // Disjunction ranges.
                        frac2 = disjunction(create_automata(v.at(0)),
                                                     create_automata(v.at(1)));
                    else if (v.size() == 2 && v.at(0).start == v.at(1).start
                             && v.at(0).end_ == v.at(1).end_) // One or more ranges.
                        frac2 = concatenation(create_automata(v.at(0)),
                                            closure(create_automata(v.at(1))));
                    else if (v.size() == 1) // Single ranges.
                        frac2 = create_automata(v.at(0));
                }
                fraction = concatenation(frac1, frac2);
                /* Get automata for exponent part. */
                Automata exp_symbol;
                string symb1 = m[6];
                string symb2 = m[7];
                range r1, r2;
                r1.start = symb1.at(0);
                r1.end_ = symb1.at(0);
                r2.start = symb2.at(0);
                r2.end_ = symb2.at(0);
                exp_symbol = disjunction(create_automata(r1),
                                         create_automata(r2));
                it = alphabet.find(m[8]);
                if (it != alphabet.end()) {
                    v = it->second;
                    if (v.size() == 2 && v.at(0).start != v.at(1).start
                        && v.at(0).end_ != v.at(1).end_) // Disjunction ranges.
                        exponent = disjunction(create_automata(v.at(0)),
                                                     create_automata(v.at(1)));
                    else if (v.size() == 2 && v.at(0).start == v.at(1).start
                             && v.at(0).end_ == v.at(1).end_) // One or more ranges.
                        exponent = concatenation(create_automata(v.at(0)),
                                            closure(create_automata(v.at(1))));
                    else if (v.size() == 1) // Single ranges.
                        exponent = create_automata(v.at(0));
                }
                exponent = concatenation(exp_symbol, exponent);
                Automata result = disjunction(first, concatenation(fraction,
                                                                    exponent));
                /* Make the last state accept this token. */
                final_NFA.find(result.end_state.id)->second.acceptance = token;
                /* Combine with final NFA. */
                final_NFA.find(0)->second.epslon_closure.push_back(result
                                                              .start_state.id);
            } else if (std::regex_search(line, m, relop)) {
                string token = m[1];
                vector<Automata> automatas;
                for (unsigned int i = 2; i < m.size(); i++) {
                    string symb = m[i];
                    bool begin_op = true;
                    Automata first, second;
                    range r;
                    for (unsigned int j = 0; j < symb.size(); j++) {
                        if (symb.at(j) != '\\' && symb.at(j) != ' ') {
                            r.start = symb.at(j);
                            r.end_ = symb.at(j);
                            // Push in basic alphabet.
                            basic_alph.insert(r);
                            if (begin_op){
                                begin_op = false;
                                first = create_automata(r);
                            } else {
                                second = create_automata(r);
                                first = concatenation(first, second);
                            }
                        } // else '\' char neglected
                    }
                    automatas.push_back(first);
                }
                Automata result = disjunction_more(automatas);
                /* Make the last state accept this token. */
                final_NFA.find(result.end_state.id)->second.acceptance = token;
                /* Combine with final NFA. */
                final_NFA.find(0)->second.epslon_closure.push_back(result
                                                              .start_state.id);
            } else if (std::regex_search(line, m, assi)) {
                string token = m[1];
                string symb = m[2];
                range r;
                r.start = symb.at(0);
                r.end_ = symb.at(0);
                // Push in basic alphabet.
                basic_alph.insert(r);
                Automata result = create_automata(r);
                /* Make the last state accept this token. */
                final_NFA.find(result.end_state.id)->second.acceptance = token;
                /* Combine with final NFA. */
                final_NFA.find(0)->second.epslon_closure.push_back(result
                                                              .start_state.id);
            }
        }
        myfile.close();
    } else cout << "Unable to open file";
}

void ThompsonConstruction::execute ()
{
    read_file();
}
