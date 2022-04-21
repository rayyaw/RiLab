#include "catch.hpp"

#include "../src/rule.h"
#include "../src/parse.h"

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using std::map;
using std::ostringstream;
using std::pair;
using std::set;
using std::string;
using std::vector;

// Unit tests for the parser

// Tests for splitCommand
TEST_CASE("Empty string returns empty vector", "[splitCommand]") {
    string cmd = "";
    vector<string> cmd_split = splitCommand(cmd);

    REQUIRE(cmd_split.empty());
}

TEST_CASE("Whitespaces are ignored when parsing - empty string", "[splitCommand]") {
    string cmd = "    ";
    vector<string> cmd_split = splitCommand(cmd);

    REQUIRE(cmd_split.empty());
}

TEST_CASE("Whitespaces are ignored when parsing - nonempty string", "[splitCommand]") {
    string cmd = " a  b c ";
    vector<string> cmd_split = splitCommand(cmd);

    REQUIRE(cmd_split.size() == 3);

    REQUIRE(cmd_split[0] == "a");
    REQUIRE(cmd_split[1] == "b");
    REQUIRE(cmd_split[2] == "c");
}

TEST_CASE("Simple string with no grouping", "[splitCommand]") {
    string cmd = "riley is gay";

    vector<string> cmd_split = splitCommand(cmd);

    REQUIRE(cmd_split.size() == 3);

    REQUIRE(cmd_split[0] == "riley");
    REQUIRE(cmd_split[1] == "is");
    REQUIRE(cmd_split[2] == "gay");  
}

TEST_CASE("Single layer of grouping - no errors", "[splitCommand]") {
    string cmd = "1 + (2 + 3) != 15";

    vector<string> cmd_split = splitCommand(cmd);

    REQUIRE(cmd_split.size() == 5);

    REQUIRE(cmd_split[0] == "1");
    REQUIRE(cmd_split[1] == "+");
    REQUIRE(cmd_split[2] == "(2 + 3)");   
    REQUIRE(cmd_split[3] == "!=");
    REQUIRE(cmd_split[4] == "15");   
}

TEST_CASE("Multiple group layers - no errors", "[splitCommand]") {
    string cmd = "1 + (2 + 3) + ((4 + 5) + 6) != 15";

    vector<string> cmd_split = splitCommand(cmd);

    REQUIRE(cmd_split.size() == 7);

    REQUIRE(cmd_split[0] == "1");
    REQUIRE(cmd_split[1] == "+");
    REQUIRE(cmd_split[2] == "(2 + 3)");  
    REQUIRE(cmd_split[3] == "+"); 
    REQUIRE(cmd_split[4] == "((4 + 5) + 6)");
    REQUIRE(cmd_split[5] == "!=");
    REQUIRE(cmd_split[6] == "15");   
}

// Testing splitCommand with errors
TEST_CASE("Never closed (", "[splitCommand]") {
    string cmd = "1 + (2 + 3) + ((4 + 5) + 6 != 15";

    REQUIRE_THROWS(splitCommand(cmd));
}

TEST_CASE("Too many closed (", "[splitCommand]") {
    string cmd = "1 + (2 + 3) + ((4 + 5) + 6)) != 15";

    REQUIRE_THROWS(splitCommand(cmd));
}

TEST_CASE("Closed ( at start", "[splitCommand]") {
    string cmd = ")";

    REQUIRE_THROWS(splitCommand(cmd));

}

TEST_CASE("Parenthesis with no whitespace around it", "[splitCommand]") {
    string cmd = "1 +(2)";

    REQUIRE_THROWS(splitCommand(cmd));
}

TEST_CASE("Single ending character", "[splitCommand]") {
    string command = "Riley _";

    vector<string> out = splitCommand(command);
    REQUIRE(out.size() == 2);
    REQUIRE(out[0] == "Riley");
    REQUIRE(out[1] == "_");
}

// parseRule tests
Env *setup_env() {
    map<string, string> variables = {{"IntVar", "Int"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}, {"TypeVar3", "Type"}};
    set<string> type_vars = {"Type"};
    vector<string> op_values = {"Int", "Int", "Int"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"}; 

    Env *env = new Env();
    env -> variables = variables;
    env -> type_names = type_names;
    env -> operators = op_names;
    env -> type_vars = type_vars;

    return env;
}

TEST_CASE("Empty string", "[parseRule]") {
    string cmd = "";

    REQUIRE_THROWS(parseRule(cmd, setup_env()));
}

TEST_CASE("Single integer literal", "[parseRule]") {
    string cmd = "12";

    Env *env = setup_env();
    RuleTree *tree = parseRule(cmd, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(Int 12)");

}

TEST_CASE("Single boolean literal", "[parseRule]") {
    string cmd = "false";

    Env *env = setup_env();
    RuleTree *tree = parseRule(cmd, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(Bool false)");

}

TEST_CASE("Single Var literal", "[parseRule]") {
    string cmd = "IntVar";

    Env *env = setup_env();
    RuleTree *tree = parseRule(cmd, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(Int IntVar)");

}

TEST_CASE("Single TypeVar literal", "[parseRule]") {
    string cmd = "Type";

    Env *env = setup_env();
    RuleTree *tree = parseRule(cmd, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(TypeVar Type)");

}

TEST_CASE("Single TypeName literal", "[parseRule]") {
    string cmd = "Real";

    Env *env = setup_env();
    RuleTree *tree = parseRule(cmd, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(TypeName Real)");

}

TEST_CASE("Multiple literals (invalid)", "[parseRule]") {
    string cmd = "10 11 12";

    Env *env = setup_env();
    REQUIRE_THROWS(parseRule(cmd, env));

}

TEST_CASE("Single invalid value", "[parseRule]") {
    string cmd = "Riley";

    Env *env = setup_env();
    REQUIRE_THROWS(parseRule(cmd, env));

}

TEST_CASE("Two Int literals", "[parseRule]") {
    string cmd = "--> 10 12";

    Env *env = setup_env();
    RuleTree *tree = parseRule(cmd, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(--> (Int 10) (Int 12))");

}

TEST_CASE("TypeVar and Float literal", "[parseRule]") {
    string cmd = "E Type 10.0";

    Env *env = setup_env();
    RuleTree *tree = parseRule(cmd, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(E (TypeVar Type) (Float 10.0))");

}

TEST_CASE("Full Nested Test (with grouping)", "[parseRule]") {
    string cmd = "+ (& 10 20.0) (|E TypeVar3)";

    Env *env = setup_env();
    RuleTree *tree = parseRule(cmd, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(+ (& (Int 10) (Float 20.0)) (|E (Type TypeVar3)))");

}

// Test incorrect number of operator arguments
TEST_CASE("Invalid number of operator arguments - user operator", "[parseRule]") {
    string cmd = "+ (& 10 20.0) (|E OtherVar) 10";

    Env *env = setup_env();
    REQUIRE_THROWS(parseRule(cmd, env));
}

TEST_CASE("Invalid number of operator arguments - default operator", "[parseRule]") {
    string cmd = "|A (& 10 20.0) (|E OtherVar)";

    Env *env = setup_env();
    REQUIRE_THROWS(parseRule(cmd, env));
}

// VarDeclare tests
TEST_CASE("Simple VarDeclare case (global variable name)", "[parseVarDeclare]") {
    string command = "Riley Int";
    Env *env = setup_env();

    pair<string, string> out = parseVarDeclare(command, env);
    REQUIRE(out.first == "Riley");
    REQUIRE(out.second == "Int");
}

TEST_CASE("Simple VarDeclare case (wildcard)", "[parseVarDeclare]") {
    string command = "Riley _";
    Env *env = setup_env();

    pair<string, string> out = parseVarDeclare(command, env);
    REQUIRE(out.first == "Riley");
    REQUIRE(out.second == "_");
}

TEST_CASE("Simple VarDeclare case (user declared type)", "[parseVarDeclare]") {
    string command = "VarName Real";
    Env *env = setup_env();

    pair<string, string> out = parseVarDeclare(command, env);
    REQUIRE(out.first == "VarName");
    REQUIRE(out.second == "Real");
}

TEST_CASE("Simple VarDeclare case (TypeVar type)", "[parseVarDeclare]") {
    string command = "Riley Type";
    Env *env = setup_env();

    pair<string, string> out = parseVarDeclare(command, env);
    REQUIRE(out.first == "Riley");
    REQUIRE(out.second == "Type");
}

TEST_CASE("Invalid VarDeclare case (Reserved name)", "[parseVarDeclare]") {
    string command = "declare Type";
    Env *env = setup_env();

    REQUIRE_THROWS(parseVarDeclare(command, env));
}

TEST_CASE("Invalid VarDeclare case (Reserved name local)", "[parseVarDeclare]") {
    string command = "IntVar Type";
    Env *env = setup_env();

    REQUIRE_THROWS(parseVarDeclare(command, env));
}

TEST_CASE("Invalid VarDeclare case (Invalid Type)", "[parseVarDeclare]") {
    string command = "var NotAType";
    Env *env = setup_env();

    REQUIRE_THROWS(parseVarDeclare(command, env));
}

// parseTypeVarDeclare tests
TEST_CASE("Simple TypeVarDeclare case (wildcard)", "[parseTypeVarDeclare]") {
    string command = "_";
    Env *env = setup_env();

    REQUIRE_THROWS(parseTypeVarDeclare(command, env));
}

TEST_CASE("Simple TypeVarDeclare case (valid)", "[parseTypeVarDeclare]") {
    string command = "Riley";
    Env *env = setup_env();

    string out = parseTypeVarDeclare(command, env);
    REQUIRE(out == "Riley");
}

TEST_CASE("Invalid TypeVarDeclare case (Reserved name)", "[parseTypeVarDeclare]") {
    string command = "declare";
    Env *env = setup_env();

    REQUIRE_THROWS(parseTypeVarDeclare(command, env));
}

TEST_CASE("Invalid TypeVarDeclare case (Reserved name local)", "[parseTypeVarDeclare]") {
    string command = "IntVar";
    Env *env = setup_env();

    REQUIRE_THROWS(parseTypeVarDeclare(command, env));
}

// parseOpDeclare tests
TEST_CASE("Invalid Operator - No Params", "[parseOpDeclare]") {
    string command = "+";
    Env *env = setup_env();

    REQUIRE_THROWS(parseOpDeclare(command, env));
}

TEST_CASE("Unary Operator", "[parseOpDeclare]") {
    string command = "^ _ Real";
    Env *env = setup_env();

    pair<string, vector<string>> out = parseOpDeclare(command, env);

    REQUIRE(out.first == "^");
    REQUIRE(out.second.size() == 2);
    REQUIRE(out.second[0] == "_");
    REQUIRE(out.second[1] == "Real");
}

TEST_CASE("Binary Operator", "[parseOpDeclare]") {
    string command = "^ Int Type Float";
    Env *env = setup_env();

    pair<string, vector<string>> out = parseOpDeclare(command, env);

    REQUIRE(out.first == "^");
    REQUIRE(out.second.size() == 3);
    REQUIRE(out.second[0] == "Int");
    REQUIRE(out.second[1] == "Type");
    REQUIRE(out.second[2] == "Float");
}

// typeCheck tests
Env *setup_type_env() {
    map<string, string> variables = {
        {"NatVar", "natural"}, 
        {"IntVar1", "Int"},
        {"IntVar2", "Int"},
        {"RealVar", "Real"}, 
        {"OtherVar1", "InputNumType"}, 
        {"OtherVar2", "InputNumType"}
    };
    
    set<string> type_vars = {"NumType", "InputNumType"};

    map<string, vector<string>> op_names = {
        {"+", {"NumType", "NumType", "NumType"}}, 
        {"/", {"Real", "NumType", "NumType"}}, 
        {"E", {"Int", "Float", "natural"}},
        {"]", {"Int", "Int", "Int"}}
    };

    set<string> type_names = {"natural", "Real"}; 

    Env *env = new Env();
    env -> variables = variables;
    env -> type_names = type_names;
    env -> operators = op_names;
    env -> type_vars = type_vars;

    return env;
}

TEST_CASE("Type checking - Single Value", "[typeCheck]") {
    string command = "10";
    Env *env = setup_type_env();

    RuleTree *to_check = parseRule(command, env);

    map<string, string> mappings = typeCheck(to_check, env, map<string, string>());

    REQUIRE(to_check -> rule_type == "Int");
    REQUIRE(mappings.empty());
}

TEST_CASE("Type checking - Basic Function, No recursion (all VarTypes)", "[typeCheck]") {
    string command = "] 10 20";
    Env *env = setup_type_env();

    RuleTree *to_check = parseRule(command, env);

    map<string, string> mappings = typeCheck(to_check, env, map<string, string>());

    REQUIRE(to_check -> rule_type == "Int");
    REQUIRE(mappings.empty());

    REQUIRE(to_check -> sub_rules[0] -> rule_type == "Int");
    REQUIRE(to_check -> sub_rules[1] -> rule_type == "Int");

}

TEST_CASE("Type checking - Basic Function, Invalid Types", "[typeCheck]") {
    string command = "] 10 20.0";
    Env *env = setup_type_env();

    RuleTree *to_check = parseRule(command, env);

    REQUIRE_THROWS(typeCheck(to_check, env, map<string, string>()));

}

TEST_CASE("Type checking - Basic Function, No recursion (Valid TypeVars)", "[typeCheck]") {
    string command = "/ 10 20";
    Env *env = setup_type_env();

    RuleTree *to_check = parseRule(command, env);

    map<string, string> mappings = typeCheck(to_check, env, map<string, string>());

    REQUIRE(to_check -> rule_type == "Real");
    REQUIRE(mappings["NumType"] == "Int");

    REQUIRE(to_check -> sub_rules[0] -> rule_type == "Int");
    REQUIRE(to_check -> sub_rules[1] -> rule_type == "Int");

}

TEST_CASE("Type checking - Basic Function, No recursion (Invalid TypeVars)", "[typeCheck]") {
    string command = "/ NatVar 20";
    Env *env = setup_type_env();

    RuleTree *to_check = parseRule(command, env);

    REQUIRE_THROWS(typeCheck(to_check, env, map<string, string>()));

}

TEST_CASE("Type checking - Basic Function, No recursion (Variable Types)", "[typeCheck]") {
    string command = "/ NatVar NatVar";
    Env *env = setup_type_env();

    RuleTree *to_check = parseRule(command, env);

    map<string, string> mappings = typeCheck(to_check, env, map<string, string>());

    REQUIRE(to_check -> rule_type == "Real");
    REQUIRE(mappings["NumType"] == "natural");

    REQUIRE(to_check -> sub_rules[0] -> rule_type == "natural");
    REQUIRE(to_check -> sub_rules[1] -> rule_type == "natural");

}

TEST_CASE("Type checking - Basic Function, No recursion (TypeVar Types)", "[typeCheck]") {
    string command = "+ OtherVar1 OtherVar2";
    Env *env = setup_type_env();

    RuleTree *to_check = parseRule(command, env);

    map<string, string> mappings = typeCheck(to_check, env, map<string, string>());

    REQUIRE(to_check -> rule_type == "InputNumType");
    REQUIRE(mappings["NumType"] == "InputNumType");

    REQUIRE(to_check -> sub_rules[0] -> rule_type == "InputNumType");
    REQUIRE(to_check -> sub_rules[1] -> rule_type == "InputNumType");

}

TEST_CASE("Type checking - Recursive (Valid)", "[typeCheck]") {
    string command = "/ (E 10.0 NatVar) (+ 10 12)";
    Env *env = setup_type_env();

    RuleTree *to_check = parseRule(command, env);

    map<string, string> mappings = typeCheck(to_check, env, map<string, string>());

    REQUIRE(to_check -> rule_type == "Real");
    REQUIRE(mappings["NumType"] == "Int");

    REQUIRE(to_check -> sub_rules[0] -> rule_type == "Int");
    REQUIRE(to_check -> sub_rules[1] -> rule_type == "Int");

}