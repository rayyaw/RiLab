#include "globals.h"

#include <map>
#include <set>
#include <string>
#include <vector>

using std::map;
using std::set;
using std::string;
using std::ostream;
using std::vector;

namespace rules {
    // Note: _ is the default TypeVar. It will match against anything.
    map<string, vector<string>> default_operators = {
        {"-->", {"Bool", "Bool", "Bool"}},  // Implication
        {"--<>", {"Bool", "Bool", "Bool"}}, // Equivalence
        
        {"!", {"Bool", "Bool"     }}, // NOT
        {"&", {"Bool", "Bool", "Bool"}}, // AND
        {"|", {"Bool", "Bool", "Bool"}}, // OR

        {"|E", {"Bool", "Bool", "Bool"}}, // there exists
    };

    set<string> default_types = {
        "Int",
        "Bool",
        "Float",
        "_"
    };

    set<string> global_reserved_names = {
        "declare", 
        "ask", 
        "rule", 
        "var", 
        "typevar", 
        "operator", 
        "typename",
        "show",
        "source",
        "literal",
        ""
    };

}