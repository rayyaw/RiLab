#include <iostream>

#include "rule.h"

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