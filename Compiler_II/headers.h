#include <iostream>
#include <string>
#include <regex>                /* For regex. */
#include <map>
#include <vector>
#include <sstream>              /* Conversion from int to string. */
#include <fstream>              /* Stream class to both read and write from/to
                                   files. */
#include <stack>
#include <string.h>
#include <set>
#include <iterator>
#include <algorithm>
#include <iomanip>              /* Handle indentation for printing the
                                   parse_result output file. */

using namespace std;

#define RESET "\033[0m"
#define RED "\033[1m\033[31m"
#define GREEN "\033[1m\033[32m"
#define CYAN "\033[1m\033[36m"
#define EPSILON "\\L"
#define END_OF_INPUT "$"
#define SYNCH "SYNCH"
#define DUMMY " "

#include "SubsetConstruction.h"
#include "DFAMinimization.h"
#include "test_input.h"
#include "Table_Calulations.h"
#include "handle_grammar.h"
#include "parser.h"

