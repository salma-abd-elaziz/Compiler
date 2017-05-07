#include "headers.h"

int main()
{
    generate_automata();
    readFile();
    handle_grammar();
    construct_table();
    print_table();
    parse();
    return 0;
}
