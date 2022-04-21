#include <iostream>

#include "globals.h"
#include "rule.h"

using namespace rules;

using std::endl;
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
    if (isTypeName(name))                                                       return true;
    if (operators.find(name)             != operators.end()                   ) return true;
    if (variables.find(name)             != variables.end()                   ) return true;
    if (global_reserved_names.find(name) != global_reserved_names.end()       ) return true;

    return false;
}

bool Env::isTypeName(string name) {
    return default_types.find(name) != default_types.end() ||
    type_vars.find(name) != type_vars.end() ||
    type_names.find(name) != type_names.end();
}

bool Env::isOpName(string name) {
    return default_operators.find(name) != default_operators.end() ||
    operators.find(name) != operators.end();
}

ostream &operator<<(ostream &os, Env env) {
    os << "Current env" << endl << "-------" << endl;

    os << endl << "Type Names (Local)" << endl << "-----" << endl;

    for (string s : env.type_names) {
        os << s << endl;
    }

    os << endl << "Type Names (Global)" << endl << "-----" << endl;

    for (string s : default_types) {
        os << s << endl;
    }

    os << endl << "TypeVars" << endl << "-----" << endl;
    for (string s : env.type_vars) {
        os << s << endl;
    }

    os << endl << "Variables" << endl << "-----" << endl;
    for (auto i = env.variables.begin(); i != env.variables.end(); ++i) {
        os << i -> first;
        os << " (of type ";
        os << i -> second;
        os << ")";
        os << endl;
    }


    os << endl << "Operators (Local)" << endl << "-----" << endl;
    for (auto i = env.operators.begin(); i != env.operators.end(); ++i) {
        os << i -> first;
        os << " : ";
        os << i -> second[0];

        os << " <- {";
        
        for (size_t j = 1; j < i -> second.size(); j++) {
            os << i -> second[j];
            os << ", ";
        }

        os << "}";
        os << endl;
    }

    os << endl << "Operators (Global)" << endl << "-----" << endl;
    for (auto i = default_operators.begin(); i != default_operators.end(); ++i) {
        os << i -> first;
        os << " : ";
        os << i -> second[0];

        os << " <- {";
        
        for (size_t j = 1; j < i -> second.size(); j++) {
            os << i -> second[j];
            os << ", ";
        }

        os << "}";
        os << endl;
    }

    os << endl << "Rules" << endl << "-----" << endl;
    for (RuleTree *r : env.rules) {
        os << *r << endl;
    }

    return os;
}