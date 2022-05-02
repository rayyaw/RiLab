#include "catch.hpp"

#include "../src/data/rule.h"
#include "../src/logic.h"
#include "../src/parse.h"

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::ostringstream;
using std::pair;
using std::set;
using std::string;
using std::vector;

Env *setupEnv() {
    map<string, string> variables = {{"a", "Int"}, {"b", "Int"}, {"c", "Int"}, 
                                    {"E", "Int"}, {"o", "Int"}, {"d", "Type"}};
    set<string> type_vars = {"Type"};
    vector<string> op_values = {"Int", "Int", "Int"}; 
    map<string, vector<string>> op_names = {{"+", op_values}};
    set<string> type_names = {}; 
    map<string, string> literals = {{"Zero", "Int"}, {"One", "Int"}};

    Env *env = new Env();
    env -> variables = variables;
    env -> type_names = type_names;
    env -> operators = op_names;
    env -> type_vars = type_vars;
    env -> literals = literals;

    return env;
}

TEST_CASE("Substitutions work correctly", "[substitute]") {
    Env *env = setupEnv();
    RuleTree *rule = parseRule("+ (+ c d) (+ a c)", env);
    RuleTree *subs1 = parseRule("E", env);
    RuleTree *subs2 = parseRule("o", env);

    RuleTree *subs_rule = substitute(env, rule, {{"c", subs1}, {"a", subs2}});

    ostringstream newrule;
    newrule << *subs_rule;

    REQUIRE(newrule.str() == "(+ (+ (Int E) (Type d)) (+ (Int o) (Int E)))");

    delete env;
    delete rule;
    delete subs1;
    delete subs2;
    delete subs_rule;

}

TEST_CASE("Substitutions work correctly (including recursive cases)", "[substitute]") {
    Env *env = setupEnv();
    RuleTree *rule = parseRule("+ (+ c d) (+ a c)", env);
    RuleTree *subs1 = parseRule("+ Zero One", env);
    RuleTree *subs2 = parseRule("+ b (+ a a)", env);

    RuleTree *subs_rule = substitute(env, rule, {{"c", subs1}, {"b", subs2}});

    ostringstream newrule;
    newrule << *subs_rule;

    REQUIRE(newrule.str() == "(+ (+ (+ (Int Zero) (Int One)) (Type d)) (+ (Int a) (+ (Int Zero) (Int One))))");

    delete env;
    delete rule;
    delete subs1;
    delete subs2;
    delete subs_rule;

}