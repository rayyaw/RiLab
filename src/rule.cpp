#include <iostream>

#include "rule.h"

using std::ostream;

ostream &operator<<(ostream &os, RuleTree r) {
    os << "(";
    os << r.rule_type;

    // Either the rule has a value, or children
    if (r.rule_value != "") {
        os << " ";
        os << r.rule_value;
    } else {
        for (RuleTree *child : r.sub_rules) {
            os << " ";
            os << *child;            
        }
    }

    os << ")";
    return os;
}