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

    /**
     * @brief Print out a rule.
     * If this is a recursive rule, it'll print (rule_op [print children here])
     * Otherwise, it'll print (rule_type rule_value)
     */
    friend ostream &operator<<(ostream &os, RuleTree r);
};

struct Env {
    set<string> type_vars;
    set<string> type_names;

    // maps from operator name to list of required type names/vars
    map<string, vector<string>> operators; 

    // maps from var name to the associated type_name
    map<string, string> variables; 

    set<RuleTree*> rules;

    /**
     * @brief Check if the given name is reserved or free in the environment.
     * 
     * @param name The name to check
     * Matches against operators, local names and the global reserved keywords.
     * @return true if the name is not in use anywhere
     * @return false otherwise
     */
    bool isReservedName(string name);

    // Check if the name is a valid TypeName or TypeVar.
    bool isTypeName(string name);

    // Check if the name is an operator name.
    bool isOpName(string name);
};