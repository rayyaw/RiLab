#include "catch.hpp"

#include "../src/rule.h"
#include "../src/parse.h"

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using std::map;
using std::set;
using std::string;
using std::ostringstream;
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

// parseRule tests
TEST_CASE("Empty string", "[parseRule]") {
    string cmd = "";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"_", "_"}; 
    map<string, vector<string>> op_names = {{"+", op_values}, {"-", op_values}, {"E", op_values}};
    set<string> type_names = {"natural", "Real"};

    REQUIRE_THROWS(parseRule(cmd, variables, type_vars, op_names, type_names));
}

TEST_CASE("Single integer literal", "[parseRule]") {
    string cmd = "12";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"};
    vector<string> op_values = {"_", "_"}; 
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
    vector<string> op_values = {"_", "_"}; 
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
    vector<string> op_values = {"_", "_"}; 
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
    vector<string> op_values = {"_", "_"}; 
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
    vector<string> op_values = {"_", "_"}; 
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
    map<string, vector<string>> op_names = {{"+",{"_", "_"}}, {"-", {"_", "_"}}, {"E", {"_", "_"}}};
    set<string> type_names = {"natural", "Real"};

    REQUIRE_THROWS(parseRule(cmd, variables, type_vars, op_names, type_names));

}

TEST_CASE("Single invalid value", "[parseRule]") {
    string cmd = "Riley";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"}; 
    map<string, vector<string>> op_names = {{"+",{"_", "_"}}, {"-", {"_", "_"}}, {"E", {"_", "_"}}};
    set<string> type_names = {"natural", "Real"};

    REQUIRE_THROWS(parseRule(cmd, variables, type_vars, op_names, type_names));

}

TEST_CASE("Two Int literals", "[parseRule]") {
    string cmd = "--> 10 12";

    map<string, string> variables = {{"IntVar", "natural"}, {"BoolVar", "Real"}, {"OtherVar", "Real"}};
    set<string> type_vars = {"TypeVarName"}; 
    map<string, vector<string>> op_names = {{"+",{"_", "_"}}, {"-", {"_", "_"}}, {"E", {"_", "_"}}};
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
    map<string, vector<string>> op_names = {{"+",{"_", "_"}}, {"-", {"_", "_"}}, {"E", {"_", "_"}}};
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
    map<string, vector<string>> op_names = {{"+",{"_", "_"}}, {"-", {"_", "_"}}, {"E", {"_", "_"}}};
    set<string> type_names = {"natural", "Real"};

    RuleTree *tree = parseRule(cmd, variables, type_vars, op_names, type_names);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(+ (& (Int 10) (Float 20.0)) (|E (TypeVarName OtherVar)))");

}

// FIXME - test invalid type matches