#pragma once

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using std::map;
using std::set;
using std::string;
using std::ostream;
using std::vector;

struct RuleTree {
    size_t rule_id;

    string rule_type;
    string rule_value;
    string rule_op;

    vector<RuleTree*> sub_rules;

    // Print out a rule.
    friend ostream &operator<<(ostream &os, RuleTree r);
};

struct Env {
    set<string> type_vars;
    set<string> type_names;

    // maps from operator name to list of required type names/vars
    map<string, vector<string>> operators; 

    // maps from var name to the associated type_name
    map<string, string> var_types; 

    set<RuleTree*> rules;
};