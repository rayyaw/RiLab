#include "catch.hpp"

#include "../src/data/rule.h"
#include "../src/data/tree.h"
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
                                    {"E", "Int"}, {"o", "Int"}, {"d", "Type"}, {"f", "_"}};
    set<string> type_vars = {"Type"};
    vector<string> op_values = {"Int", "Int", "Int"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"IsInt", {"Bool", "_"}}};
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

TEST_CASE("Failed generalization: Incompatible Operators", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *specific = parseRule("--> false true", env);
    RuleTree *general = parseRule("--<> true true", env);

    REQUIRE_THROWS(generalize(env, general, specific, map<string, RuleTree*>()));

    delete env;
    delete specific;
    delete general;
} 

TEST_CASE("Failed generalization: Can't generalize from Var to Literal", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *specific = parseRule("+ a b", env);
    RuleTree *general = parseRule("+ Zero b", env);

    REQUIRE_THROWS(generalize(env, general, specific, map<string, RuleTree*>()));

    delete env;
    delete specific;
    delete general;
} 

TEST_CASE("Failed generalization: Can't generalize from TypeName to TypeVar (simple)", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *specific = parseRule("d", env);
    RuleTree *general = parseRule("b", env);

    REQUIRE_THROWS(generalize(env, general, specific, map<string, RuleTree*>()));

    delete env;
    delete specific;
    delete general;
}

TEST_CASE("Failed generalization: Can't generalize from TypeName to TypeVar", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *specific = parseRule("+ a d", env);
    RuleTree *general = parseRule("+ a b", env);

    REQUIRE_THROWS(generalize(env, general, specific, map<string, RuleTree*>()));

    delete env;
    delete specific;
    delete general;
}

TEST_CASE("Failed generalization: Can't generalize already assigned types", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *specific = parseRule("+ a b", env);
    RuleTree *general = parseRule("+ a d", env);
    RuleTree *sub_type = parseRule("a", env);

    REQUIRE_THROWS(generalize(env, general, specific, {{"d", sub_type}}));

    delete env;
    delete sub_type;
    delete specific;
    delete general;
} 

// Successful generalization tests (lit, var, typevar, _ -> lit, var, typevar, _)
TEST_CASE("Successful generalization: Literal generalizes to itself", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *specific = parseRule("+ Zero One", env);
    RuleTree *general = parseRule("+ Zero One", env);

    map<string, RuleTree*> subs = generalize(env, general, specific, map<string, RuleTree*>());

    REQUIRE(subs.empty());

    delete env;
    delete specific;
    delete general;
} 

TEST_CASE("Successful generalization: Literal generalizes to Var / TypeVar", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *specific = parseRule("+ Zero One", env);
    RuleTree *general = parseRule("+ a d", env);

    map<string, RuleTree*> subs = generalize(env, general, specific, map<string, RuleTree*>());

    REQUIRE(subs.size() == 2);
    REQUIRE(subs["a"] -> rule_value == "Zero");
    REQUIRE(subs["d"] -> rule_value == "One");

    delete env;
    delete specific;
    delete general;
} 

TEST_CASE("Successful generalization: Literal generalizes to _", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *specific = parseRule("+ Zero One", env);
    RuleTree *general = parseRule("+ a f", env);

    map<string, RuleTree*> subs = generalize(env, general, specific, map<string, RuleTree*>());

    REQUIRE(subs.size() == 2);
    REQUIRE(subs["a"] -> rule_value == "Zero");
    REQUIRE(subs["f"] -> rule_value == "One");

    delete env;
    delete specific;
    delete general;
} 

TEST_CASE("Successful generalization: Var generalizes to Var", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *specific = parseRule("+ a b", env);
    RuleTree *general = parseRule("+ a c", env);

    map<string, RuleTree*> subs = generalize(env, general, specific, map<string, RuleTree*>());

    REQUIRE(subs.size() == 2);
    REQUIRE(subs["a"] -> rule_value == "a");
    REQUIRE(subs["c"] -> rule_value == "b");

    delete env;
    delete specific;
    delete general;
} 

TEST_CASE("Successful generalization: Var generalizes to TypeVar", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *specific = parseRule("+ a b", env);
    RuleTree *general = parseRule("+ a d", env);

    map<string, RuleTree*> subs = generalize(env, general, specific, map<string, RuleTree*>());

    REQUIRE(subs.size() == 2);
    REQUIRE(subs["a"] -> rule_value == "a");
    REQUIRE(subs["d"] -> rule_value == "b");

    delete env;
    delete specific;
    delete general;
} 

TEST_CASE("Successful generalization: Var generalizes to _", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *specific = parseRule("+ a b", env);
    RuleTree *general = parseRule("+ a f", env);

    map<string, RuleTree*> subs = generalize(env, general, specific, map<string, RuleTree*>());

    REQUIRE(subs.size() == 2);
    REQUIRE(subs["a"] -> rule_value == "a");
    REQUIRE(subs["f"] -> rule_value == "b");

    delete env;
    delete specific;
    delete general;
} 

TEST_CASE("Successful generalization: TypeVar generalizes to TypeVar", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *specific = parseRule("+ a d", env);
    RuleTree *general = parseRule("+ a d", env);

    map<string, RuleTree*> subs = generalize(env, general, specific, map<string, RuleTree*>());

    REQUIRE(subs.size() == 2);
    REQUIRE(subs["a"] -> rule_value == "a");
    REQUIRE(subs["d"] -> rule_value == "d");

    delete env;
    delete specific;
    delete general;
} 

TEST_CASE("Successful generalization: TypeVar generalizes to _", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *specific = parseRule("+ a d", env);
    RuleTree *general = parseRule("+ a f", env);

    map<string, RuleTree*> subs = generalize(env, general, specific, map<string, RuleTree*>());

    REQUIRE(subs.size() == 2);
    REQUIRE(subs["a"] -> rule_value == "a");
    REQUIRE(subs["f"] -> rule_value == "d");

    delete env;
    delete specific;
    delete general;
} 

// Generalizations from a variable to a full expression (including fails)
TEST_CASE("Successful generalization: Nested case", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *specific = parseRule("+ a (+ b Zero)", env);
    RuleTree *general = parseRule("+ a c", env);

    map<string, RuleTree*> subs = generalize(env, general, specific, map<string, RuleTree*>());

    REQUIRE(subs.size() == 2);
    REQUIRE(subs["c"] -> rule_op == "+");
    REQUIRE(subs["c"] -> sub_rules.size() == 2);
    REQUIRE(subs["c"] -> sub_rules[0] -> rule_value == "b");
    REQUIRE(subs["c"] -> sub_rules[1] -> rule_value == "Zero");
    REQUIRE(subs["a"] -> rule_value == "a");

    delete env;
    delete specific;
    delete general;
} 

// applyRule tests
TEST_CASE("Rule application: Commutative Addition", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *victim = parseRule("+ c (+ b Zero)", env);
    RuleTree *apply = parseRule("--<> (+ a b) (+ b a)", env);

    RuleTree *applied = applyRule(env, apply, victim, true);

    ostringstream out;
    out << *applied;
    REQUIRE(out.str() == "(+ (+ (Int b) (Int Zero)) (Int c))");

    delete env;
    delete victim;
    delete apply;
    delete applied;
}

TEST_CASE("Rule application: Associative Addition", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *victim = parseRule("+ a (+ b c)", env);
    RuleTree *apply = parseRule("--<> (+ a b) (+ b a)", env);

    RuleTree *applied = applyRule(env, apply, victim, false);

    ostringstream out;
    out << *applied;
    REQUIRE(out.str() == "(+ (+ (Int b) (Int c)) (Int a))");

    delete env;
    delete victim;
    delete apply;
    delete applied;
}

TEST_CASE("Incorrect direction of -->", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *victim = parseRule("IsInt (+ c 1)", env);
    RuleTree *apply = parseRule("--> (IsInt a) (IsInt (+ a 1))", env);

    REQUIRE_THROWS(applyRule(env, apply, victim, false));

    delete env;
    delete victim;
    delete apply;
}

TEST_CASE("Invalid operator, can't apply", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *victim = parseRule("--<> (+ c 1) (+ b 2)", env);
    RuleTree *apply = parseRule("--> (+ a 1) (+ b 2)", env);

    REQUIRE_THROWS(applyRule(env, apply, victim, false));

    delete env;
    delete victim;
    delete apply;
}

TEST_CASE("Rule application: Integer Inclusion", "[generalize]") {
    Env *env = setupEnv();
    RuleTree *victim = parseRule("IsInt (+ c 1)", env);
    RuleTree *apply = parseRule("--> (IsInt a) (IsInt (+ a 1))", env);

    RuleTree *applied = applyRule(env, apply, victim, true);

    ostringstream out;
    out << *applied;
    REQUIRE(out.str() == "(IsInt (Int c))");

    delete env;
    delete victim;
    delete apply;
    delete applied;
}

// runAskWorker tests
Env *setupMathEnv() {
    Env *env = new Env();
    parseStatement("source tests/nat.rilab", env);
    return env;
}

TEST_CASE("Simple proof (2 in N)", "[runAskWorker]") {
    Env *env = setupMathEnv();

    ProofTreeNode *root = new ProofTreeNode();

    RuleTree *to_prove = parseRule("InNatural Two", env);
    root -> to_prove_remainder = new RuleTree(*to_prove);
    root -> applied_rule = parseRule("--<> (InNatural Two) (InNatural (S (S Zero)))", env);

    ProofTreeNode *leaf = runAskWorker(env, 5, root);

    string proof = showProof(leaf);

    REQUIRE(proof == "==> (InNatural (Natural Zero))\nApply rule (--> (InNatural (Natural x)) (InNatural (S (Natural x))))\n\n==> (InNatural (S (Natural Zero)))\nApply rule (--> (InNatural (Natural x)) (InNatural (S (Natural x))))\n\n==> (InNatural (S (S (Natural Zero))))\nApply rule (--<> (InNatural (Natural Two)) (InNatural (S (S (Natural Zero)))))\n\n");

    delete to_prove;
    delete root -> applied_rule;
    delete root;
    delete env;
}