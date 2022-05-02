#include "catch.hpp"

#include "../src/data/rule.h"
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
Env *setupEnv() {
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

    Env *env = setupEnv();

    REQUIRE_THROWS(parseRule(cmd, env));

    delete env;

}

TEST_CASE("Single integer literal", "[parseRule]") {
    string cmd = "12";

    Env *env = setupEnv();
    RuleTree *tree = parseRule(cmd, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(Int 12)");

    delete tree;
    delete env;

}

TEST_CASE("Single boolean literal", "[parseRule]") {
    string cmd = "false";

    Env *env = setupEnv();
    RuleTree *tree = parseRule(cmd, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(Bool false)");

    delete tree;
    delete env;

}

TEST_CASE("Single Var literal", "[parseRule]") {
    string cmd = "IntVar";

    Env *env = setupEnv();
    RuleTree *tree = parseRule(cmd, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(Int IntVar)");

    delete tree;
    delete env;

}

TEST_CASE("Single TypeVar literal", "[parseRule]") {
    string cmd = "Type";

    Env *env = setupEnv();
    RuleTree *tree = parseRule(cmd, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(TypeVar Type)");

    delete tree;
    delete env;

}

TEST_CASE("Single TypeName literal", "[parseRule]") {
    string cmd = "Real";

    Env *env = setupEnv();
    RuleTree *tree = parseRule(cmd, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(TypeName Real)");

    delete tree;
    delete env;

}

TEST_CASE("Multiple literals (invalid)", "[parseRule]") {
    string cmd = "10 11 12";

    Env *env = setupEnv();
    REQUIRE_THROWS(parseRule(cmd, env));

    delete env;

}

TEST_CASE("Single invalid value", "[parseRule]") {
    string cmd = "Riley";

    Env *env = setupEnv();
    REQUIRE_THROWS(parseRule(cmd, env));

    delete env;

}

TEST_CASE("Two Int literals", "[parseRule]") {
    string cmd = "--> true true";

    Env *env = setupEnv();
    RuleTree *tree = parseRule(cmd, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(--> (Bool true) (Bool true))");

    delete tree;
    delete env;

}

TEST_CASE("TypeVar and Float literal", "[parseRule]") {
    string cmd = "E Type 10.0";

    Env *env = setupEnv();
    RuleTree *tree = parseRule(cmd, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(E (TypeVar Type) (Float 10.0))");

    delete tree;
    delete env;

}

TEST_CASE("Full Nested Test (with grouping)", "[parseRule]") {
    string cmd = "+ (& 10 20.0) (|E TypeVar3 false)";

    Env *env = setupEnv();
    RuleTree *tree = parseRule(cmd, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(+ (& (Int 10) (Float 20.0)) (|E (Type TypeVar3) (Bool false)))");

    delete tree;
    delete env;

}

// Test incorrect number of operator arguments
TEST_CASE("Invalid number of operator arguments - user operator", "[parseRule]") {
    string cmd = "+ (& 10 20.0) (|E OtherVar) 10";

    Env *env = setupEnv();
    REQUIRE_THROWS(parseRule(cmd, env));

    delete env;
}

TEST_CASE("Invalid number of operator arguments - default operator", "[parseRule]") {
    string cmd = "|A (& 10 20.0) (|E OtherVar)";

    Env *env = setupEnv();
    REQUIRE_THROWS(parseRule(cmd, env));

    delete env;
}

TEST_CASE("Recursive Rule Parse", "[parseRule]") {
    string command = "--<> (+ 1 2) (+ 2 1)";
    Env *env = setupEnv();

    RuleTree *tree = parseRule(command, env);

    ostringstream oss = ostringstream();
    oss << *tree;

    string tree_val = oss.str();
    REQUIRE(tree_val == "(--<> (+ (Int 1) (Int 2)) (+ (Int 2) (Int 1)))");

    delete tree;
    delete env;
}

// VarDeclare tests
TEST_CASE("Simple VarDeclare case (global variable name)", "[parseVarDeclare]") {
    string command = "Riley Int";
    Env *env = setupEnv();

    pair<string, string> out = parseVarDeclare(command, env);
    REQUIRE(out.first == "Riley");
    REQUIRE(out.second == "Int");

    delete env;
}

TEST_CASE("Simple VarDeclare case (wildcard)", "[parseVarDeclare]") {
    string command = "Riley _";
    Env *env = setupEnv();

    pair<string, string> out = parseVarDeclare(command, env);
    REQUIRE(out.first == "Riley");
    REQUIRE(out.second == "_");

    delete env;
}

TEST_CASE("Simple VarDeclare case (user declared type)", "[parseVarDeclare]") {
    string command = "VarName Real";
    Env *env = setupEnv();

    pair<string, string> out = parseVarDeclare(command, env);
    REQUIRE(out.first == "VarName");
    REQUIRE(out.second == "Real");

    delete env;
}

TEST_CASE("Simple VarDeclare case (TypeVar type)", "[parseVarDeclare]") {
    string command = "Riley Type";
    Env *env = setupEnv();

    pair<string, string> out = parseVarDeclare(command, env);
    REQUIRE(out.first == "Riley");
    REQUIRE(out.second == "Type");

    delete env;
}

TEST_CASE("Invalid VarDeclare case (Reserved name)", "[parseVarDeclare]") {
    string command = "declare Type";
    Env *env = setupEnv();

    REQUIRE_THROWS(parseVarDeclare(command, env));

    delete env;
}

TEST_CASE("Invalid VarDeclare case (Reserved name local)", "[parseVarDeclare]") {
    string command = "IntVar Type";
    Env *env = setupEnv();

    REQUIRE_THROWS(parseVarDeclare(command, env));

    delete env;
}

TEST_CASE("Invalid VarDeclare case (Invalid Type)", "[parseVarDeclare]") {
    string command = "var NotAType";
    Env *env = setupEnv();

    REQUIRE_THROWS(parseVarDeclare(command, env));

    delete env;
}

// parseTypeVarDeclare tests
TEST_CASE("Simple TypeVarDeclare case (wildcard)", "[parseTypeVarDeclare]") {
    string command = "_";
    Env *env = setupEnv();

    REQUIRE_THROWS(parseTypeVarName(command, env));

    delete env;
}

TEST_CASE("Simple TypeVarDeclare case (valid)", "[parseTypeVarDeclare]") {
    string command = "Riley";
    Env *env = setupEnv();

    string out = parseTypeVarName(command, env);
    REQUIRE(out == "Riley");

    delete env;
}

TEST_CASE("Invalid TypeVarDeclare case (Reserved name)", "[parseTypeVarDeclare]") {
    string command = "declare";
    Env *env = setupEnv();

    REQUIRE_THROWS(parseTypeVarName(command, env));

    delete env;
}

TEST_CASE("Invalid TypeVarDeclare case (Reserved name local)", "[parseTypeVarDeclare]") {
    string command = "IntVar";
    Env *env = setupEnv();

    REQUIRE_THROWS(parseTypeVarName(command, env));

    delete env;
}

// parseOpDeclare tests
TEST_CASE("Invalid Operator - No Params", "[parseOpDeclare]") {
    string command = "+";
    Env *env = setupEnv();

    REQUIRE_THROWS(parseOpDeclare(command, env));

    delete env;
}

TEST_CASE("Unary Operator", "[parseOpDeclare]") {
    string command = "^ _ Real";
    Env *env = setupEnv();

    pair<string, vector<string>> out = parseOpDeclare(command, env);

    REQUIRE(out.first == "^");
    REQUIRE(out.second.size() == 2);
    REQUIRE(out.second[0] == "_");
    REQUIRE(out.second[1] == "Real");

    delete env;
}

TEST_CASE("Binary Operator", "[parseOpDeclare]") {
    string command = "^ Int Type Float";
    Env *env = setupEnv();

    pair<string, vector<string>> out = parseOpDeclare(command, env);

    REQUIRE(out.first == "^");
    REQUIRE(out.second.size() == 3);
    REQUIRE(out.second[0] == "Int");
    REQUIRE(out.second[1] == "Type");
    REQUIRE(out.second[2] == "Float");

    delete env;
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
        {"]", {"Int", "Int", "Int"}},
        {"[", {"Bool", "_", "_"}}
    };

    set<string> type_names = {"natural", "Real"}; 

    Env *env = new Env();
    env -> variables = variables;
    env -> type_names = type_names;
    env -> operators = op_names;
    env -> type_vars = type_vars;
    env -> rules = set<RuleTree*>();

    return env;
}

TEST_CASE("Type checking - Single Value", "[typeCheck]") {
    string command = "10";
    Env *env = setup_type_env();

    RuleTree *to_check = parseRule(command, env);

    map<string, string> mappings = typeCheck(to_check, env, map<string, string>());

    REQUIRE(to_check -> rule_type == "Int");
    REQUIRE(mappings.empty());

    delete to_check;
    delete env;
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

    delete to_check;
    delete env;

}

TEST_CASE("Type checking - Basic Function, Invalid Types", "[typeCheck]") {
    string command = "] 10 20.0";
    Env *env = setup_type_env();

    RuleTree *to_check = parseRule(command, env);

    REQUIRE_THROWS(typeCheck(to_check, env, map<string, string>()));

    delete to_check;
    delete env;

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

    delete to_check;
    delete env;

}

TEST_CASE("Type checking - Basic Function, No recursion (Invalid TypeVars)", "[typeCheck]") {
    string command = "/ NatVar 20";
    Env *env = setup_type_env();

    RuleTree *to_check = parseRule(command, env);

    REQUIRE_THROWS(typeCheck(to_check, env, map<string, string>()));

    delete to_check;
    delete env;

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

    delete to_check;
    delete env;

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

    delete to_check;
    delete env;

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

    delete to_check;
    delete env;

}

// parseStatement tests
TEST_CASE("Invalid statement", "[parseStatement]") {
    string command = "";
    Env *env = setup_type_env();

    string output = parseStatement(command, env);
    REQUIRE(output == "\n");

    delete env;
}

TEST_CASE("Invalid statement with Throw", "[parseStatement]") {
    string command = "E A";
    Env *env = setup_type_env();

    REQUIRE_THROWS(parseStatement(command, env));

    delete env;
}

TEST_CASE("Invalid Declare statement", "[parseStatement]") {
    string command = "Declare A";
    Env *env = setup_type_env();

    REQUIRE_THROWS(parseStatement(command, env));

    delete env;
}

TEST_CASE("Show statement", "[parseStatement]") {
    string command = "show";
    Env *env = setup_type_env();

    string output = parseStatement(command, env);
    string expected = "Current env\n-------\n\nType Names (Local)\n-----\nReal\nnatural\n\nType Names (Global)\n-----\nBool\nFloat\nInt\n_\n\nTypeVars\n-----\nInputNumType\nNumType\n\nVariables\n-----\nIntVar1 (of type Int)\nIntVar2 (of type Int)\nNatVar (of type natural)\nOtherVar1 (of type InputNumType)\nOtherVar2 (of type InputNumType)\nRealVar (of type Real)\n\nLiterals\n-----\n\nOperators (Local)\n-----\n+ : NumType <- {NumType, NumType, }\n/ : Real <- {NumType, NumType, }\nE : Int <- {Float, natural, }\n[ : Bool <- {_, _, }\n] : Int <- {Int, Int, }\n\nOperators (Global)\n-----\n! : Bool <- {Bool, }\n& : Bool <- {Bool, Bool, }\n--<> : Bool <- {Bool, Bool, }\n--> : Bool <- {Bool, Bool, }\n| : Bool <- {Bool, Bool, }\n|E : Bool <- {Bool, Bool, }\n\nRules\n-----\n";

    REQUIRE(output == expected);

    delete env;
}

TEST_CASE("Declare TypeVar statement", "[parseStatement]") {
    string command = "declare typevar Truthy";
    Env *env = setup_type_env();

    string output = parseStatement(command, env);

    REQUIRE(output == "Added TypeVar Truthy.\n");
    REQUIRE(env -> type_vars.find("Truthy") != env -> type_vars.end());

    delete env;
}

TEST_CASE("Declare Variable statement", "[parseStatement]") {
    string command = "declare var Riley NumType";
    Env *env = setup_type_env();

    string output = parseStatement(command, env);

    REQUIRE(output == "Added variable Riley of type NumType.\n");
    REQUIRE(env -> variables["Riley"] == "NumType");

    delete env;
}

TEST_CASE("Declare Operator statement", "[parseStatement]") {
    string command = "declare operator T Float Int Int Int";
    Env *env = setup_type_env();

    string output = parseStatement(command, env);

    REQUIRE(output == "Added Operator T with types out=Float, in=Int, Int, Int, \n");

    vector<string> params = env -> operators["T"];

    REQUIRE(params.size() == 4);
    REQUIRE(params[0] == "Float");
    REQUIRE(params[1] == "Int");
    REQUIRE(params[2] == "Int");
    REQUIRE(params[3] == "Int");

    delete env;
}

TEST_CASE("Declare Rule statement", "[parseStatement]") {
    string command = "declare rule [ (+ 1 2) (+ 2 1)";
    Env *env = setup_type_env();

    string output = parseStatement(command, env);

    REQUIRE(output == "Added Rule ([ (+ (Int 1) (Int 2)) (+ (Int 2) (Int 1)))\n");
    REQUIRE(env -> rules.size() == 1);

    delete env;
}

TEST_CASE("Declare Literal statement", "[parseStatement]") {
    string command = "declare literal Zero Int";
    Env *env = setup_type_env();

    string output = parseStatement(command, env);

    REQUIRE(output == "Added literal Zero of type Int.\n");

    delete env;
}

TEST_CASE("Literal Rule statement", "[parseStatement]") {
    string command = "declare rule [ Zero NumType";
    Env *env = setup_type_env();

    parseStatement("declare literal Zero Int", env);
    string output = parseStatement(command, env);

    REQUIRE(output == "Added Rule ([ (Int Zero) (TypeVar NumType))\n");

    delete env;
}