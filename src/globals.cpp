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
        {"-->", {"_", "_", "_"}},  // Implication
        {"--<>", {"_", "_", "_"}}, // Equivalence
        
        {"!", {"_", "_"     }}, // NOT
        {"&", {"_", "_", "_"}}, // AND
        {"|", {"_", "_", "_"}}, // OR

        {":", {"_", "_", "_"}},  // such that
        {"|A", {"_", "_"    }}, // for all
        {"|E", {"_", "_"    }}, // there exists

        {"in", {"_", "_", "_"}}  // Set/Type Inclusion
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