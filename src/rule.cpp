#include <iostream>

#include "globals.h"
#include "rule.h"

using namespace rules;

using std::ostream;

ostream &operator<<(ostream &os, RuleTree r) {
    os << "(";
    
    // Either the rule has a value, or children
    if (r.rule_value != "") {
        os << r.rule_type;
        os << " ";
        os << r.rule_value;
    } else {
        os << r.rule_op;
        for (RuleTree *child : r.sub_rules) {
            os << " ";
            os << *child;            
        }
    }

    os << ")";
    return os;
}

bool Env::isReservedName(string name) {
    if (default_operators.find(name)     != default_operators.end()           ) return true;
    if (type_vars.find(name)             != type_vars.end()                   ) return true;
    if (type_names.find(name)            != type_names.end()                  ) return true;
    if (operators.find(name)             != operators.end()                   ) return true;
    if (variables.find(name)             != variables.end()                   ) return true;
    if (global_reserved_names.find(name) != global_reserved_names.end()       ) return true;

    return false;
}