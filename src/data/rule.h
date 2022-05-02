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
    string rule_type;
    string rule_value;
    string rule_op;

    vector<RuleTree*> sub_rules;

    RuleTree();
    RuleTree(const RuleTree &other);
    RuleTree &operator=(const RuleTree &other);
    ~RuleTree();

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
    // Literals are like variables except you can't substitute one for another
    map<string, string> literals;

    set<RuleTree*> rules;

    // Used for running an ask
    RuleTree *ask_rule;
    map<string, string> type_var_subs;

    Env();
    ~Env();

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

    /**
     * @brief Print out an env, including all rules, vars, etc.
     */
    friend ostream &operator<<(ostream &os, Env env);
};