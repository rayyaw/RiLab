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
TEST_CASE("Empty string", "[parseRule]") {
    string cmd = "";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"_", "_", "_"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"};

    REQUIRE_THROWS(parseRule(cmd, variables, type_vars, op_names, type_names));
}

TEST_CASE("Single integer literal", "[parseRule]") {
    string cmd = "12";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"_", "_", "_"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"};

    RuleTree *tree = parseRule(cmd, variables, type_vars, op_names, type_names);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(Int 12)");

}

TEST_CASE("Single boolean literal", "[parseRule]") {
    string cmd = "false";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"_", "_", "_"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"};

    RuleTree *tree = parseRule(cmd, variables, type_vars, op_names, type_names);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(Bool false)");

}

TEST_CASE("Single Var literal", "[parseRule]") {
    string cmd = "IntVar";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"_", "_", "_"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"};

    RuleTree *tree = parseRule(cmd, variables, type_vars, op_names, type_names);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(natural IntVar)");

}

TEST_CASE("Single TypeVar literal", "[parseRule]") {
    string cmd = "TypeVarName";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"_", "_", "_"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"};

    RuleTree *tree = parseRule(cmd, variables, type_vars, op_names, type_names);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(TypeVar TypeVarName)");

}

TEST_CASE("Single TypeName literal", "[parseRule]") {
    string cmd = "Real";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"_", "_", "_"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"};

    RuleTree *tree = parseRule(cmd, variables, type_vars, op_names, type_names);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(TypeName Real)");

}

TEST_CASE("Multiple literals (invalid)", "[parseRule]") {
    string cmd = "10 11 12";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"_", "_", "_"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"};

    REQUIRE_THROWS(parseRule(cmd, variables, type_vars, op_names, type_names));

}

TEST_CASE("Single invalid value", "[parseRule]") {
    string cmd = "Riley";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"_", "_", "_"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"};

    REQUIRE_THROWS(parseRule(cmd, variables, type_vars, op_names, type_names));

}

TEST_CASE("Two Int literals", "[parseRule]") {
    string cmd = "--> 10 12";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"_", "_", "_"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"};

    RuleTree *tree = parseRule(cmd, variables, type_vars, op_names, type_names);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(--> (Int 10) (Int 12))");

}

TEST_CASE("TypeVar and Float literal", "[parseRule]") {
    string cmd = "E TypeVarName 10.0";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"_", "_", "_"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"};

    RuleTree *tree = parseRule(cmd, variables, type_vars, op_names, type_names);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(E (TypeVar TypeVarName) (Float 10.0))");

}

TEST_CASE("Full Nested Test (with grouping)", "[parseRule]") {
    string cmd = "+ (& 10 20.0) (|E OtherVar)";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "TypeVarName"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"_", "_", "_"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"};

    RuleTree *tree = parseRule(cmd, variables, type_vars, op_names, type_names);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(+ (& (Int 10) (Float 20.0)) (|E (TypeVarName OtherVar)))");

}

// Test incorrect number of operator arguments
TEST_CASE("Invalid number of operator arguments - user operator", "[parseRule]") {
    string cmd = "+ (& 10 20.0) (|E OtherVar) 10";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"_", "_", "_"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"};

    REQUIRE_THROWS(parseRule(cmd, variables, type_vars, op_names, type_names));
}

TEST_CASE("Invalid number of operator arguments - default operator", "[parseRule]") {
    string cmd = "|A (& 10 20.0) (|E OtherVar)";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "TypeVarName"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"_", "_", "_"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"};

    REQUIRE_THROWS(parseRule(cmd, variables, type_vars, op_names, type_names));
}

// Invalid type match tests
TEST_CASE("Invalid operands to user function (simple)", "[parseRule]") {
    string cmd = "+ 10.0 12";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"Int", "Int", "Int"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"};

    REQUIRE_THROWS(parseRule(cmd, variables, type_vars, op_names, type_names));

}

TEST_CASE("Invalid operands to user function (recursive)", "[parseRule]") {
    string cmd = "+ (--> 10 IntVar) 12";

    map<string, string> variables = {{"IntVar", "Int"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"Int", "Int", "Int"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"};

    REQUIRE_THROWS(parseRule(cmd, variables, type_vars, op_names, type_names));

}

// VarDeclare tests
Env *setup_env() {
    map<string, string> variables = {{"IntVar", "Int"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
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