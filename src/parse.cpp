#include "data/globals.h"
#include "data/rule.h"
#include "data/utils.h"
#include "parse.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <string>
#include <vector>

using std::endl;
using std::getline;
using std::ifstream;
using std::map;
using std::ostringstream;
using std::set;
using std::string;
using std::vector;

using namespace rules;

string parseStatement(string command, Env *env) {
    // Find the first keyword.
    int first_space = charPos(command, ' ', 1);
    env -> ask_rule = nullptr;

    ostringstream out = ostringstream();

    string first_word = command.substr(0, first_space);

    // Show the current environment.
    if (command == "show") {
        out << *env;
        return out.str();
    }

    if (first_space == -1) {
        return "\n";
    }

    // If we have an ask, check for validity then return it
    if (first_word == "ask") {
        string remainder = command.substr(first_space + 1);
        RuleTree *ask_rule = parseRule(remainder, env);

        typeCheck(ask_rule, env, map<string, string>());

        if (ask_rule -> rule_type != "Bool") {
            throw "IllegalArgumentException: Rules must be of type Bool";
        }

        env -> ask_rule = ask_rule;

        out << "Asking " << *ask_rule << endl;
        return out.str();
    }

    // Source the code from a file
    if (first_word == "source") {
        string remainder = command.substr(first_space + 1);

        ifstream f;
        f.open(remainder);

        if (!f.is_open()) {
            throw "FileNotFoundException: Please check the file exists.";
        }

        string line;
        ostringstream output;
        while (!f.eof()) {
            getline(f, line);
            output << parseStatement(line, env);
        }

        return output.str();
    }

    if (first_word == "declare") {
        int second_space = charPos(command, ' ', 2);

        if (second_space == -1) {
            throw "ParseException: Invalid use of 'declare' keyword. See the README for more details.";
        }

        string second_word = command.substr(first_space + 1, second_space - first_space - 1);
        string remainder = command.substr(second_space + 1);

        // VarDeclare
        if (second_word == "var") {
            pair<string, string> var = parseVarDeclare(remainder, env);
            env -> variables[var.first] = var.second;

            out << "Added variable " << var.first << " of type " << var.second << "." << endl;
            return out.str();
        }

        if (second_word == "literal") {
            pair<string, string> var = parseVarDeclare(remainder, env);
            env -> literals[var.first] = var.second;

            out << "Added literal " << var.first << " of type " << var.second << "." << endl;
            return out.str();
        }

        // TypeDeclare
        if (second_word == "type") {
            string type = parseTypeVarName(remainder, env);
            env -> type_names.insert(type);

            out << "Added Type " << type << "." << endl;
            return out.str();
        }

        // TypeVarDeclare
        if (second_word == "typevar") {
            string type_var = parseTypeVarName(remainder, env);
            env -> type_vars.insert(type_var);

            out << "Added TypeVar " << type_var << "." << endl;
            return out.str();
        }

        // OpDeclare
        if (second_word == "operator") {
            pair<string, vector<string>> op_specs = parseOpDeclare(remainder, env);
            env -> operators[op_specs.first] = op_specs.second;

            out << "Added Operator " << op_specs.first << " with types out=" << 
            op_specs.second[0] << ", in=";

            for (size_t i = 1; i < op_specs.second.size(); i++) {
                out << op_specs.second[i] << ", ";
            }

            out << endl;
            return out.str();
        }

        // RuleDeclare
        if (second_word == "rule") {
            RuleTree *rule = parseRule(remainder, env);
            typeCheck(rule, env, map<string, string>());

            if (rule -> rule_type != "Bool") {
                delete rule;
                throw "IllegalArgumentException: Declared rules must be of type Bool";
            }

            env -> rules.insert(rule);

            out << "Added Rule " << *rule << endl;
            return out.str();
        }
    }

    throw "ParseException: Unrecognized Command Name. See the README for more details.";

}

RuleTree *parseRule(string command, Env *env) {
    map<string, string> variables = env -> variables;
    set<string> type_vars = env -> type_vars;

    map<string, vector<string>> op_names = env -> operators;
    set<string> type_names = env -> type_names;

    vector<string> command_parts = splitCommand(command);
    map<string, string> literals = env -> literals;

    if (command_parts.empty()) {
        throw "ParseException: Empty input is invalid";
    }

    RuleTree *current = new RuleTree();

    // Assume a rule is of the form (op a b ...) like how Haskell functions are applied

    // If this is an operator, split and recurse.
    if (default_operators.find(command_parts[0]) != default_operators.end() ||
    op_names.find(command_parts[0]) != op_names.end()) {
        current -> rule_op = command_parts[0];

        vector<string> op_params;
        if (default_operators.find(command_parts[0]) != default_operators.end()) {
            op_params = default_operators[command_parts[0]];
        } else {
            op_params = op_names[command_parts[0]];
        } 

        // Check that the function argument count is correct
        if (op_params.size() != command_parts.size()) {
            delete current;
            throw "IllegalArgumentException: Invalid number of arguments passed to function " + command_parts[0];
        }

        current -> rule_type = op_params[0];

        for (size_t i = 1; i < command_parts.size(); i++) {
            string next_command = command_parts[i];

            // Remove grouping if needed
            if (next_command[0] == '(') {
                next_command = next_command.substr(1, next_command.length() - 2);
            }

            current -> sub_rules.push_back(
                parseRule(next_command, env)
            );
        }
    }

    // If no operator was found, check this is a variable of some kind and store it.
    else {
        // Check that there's only a single extra command_part, with no grouping.
        if (command_parts.size() != 1) {
            delete current;
            throw "ParseException: Single values must be a single value at " + command;
        } else if (command_parts[0][0] == '(') {
            delete current;
            throw "ParseException: Single values may not contain grouping at " + command_parts[0];
        }

        // Check whether this is a boolean.
        if (command_parts[0] == "true" || command_parts[0] == "false") {
            current -> rule_type = "Bool";
            current -> rule_value = command_parts[0];
            return current;            
        }

        // Check whether this is an int.
        if (isInt(command_parts[0])) {
            current -> rule_type = "Int";
            current -> rule_value = command_parts[0];
            return current;               
        }

        // Check whether this is a float.
        if (isFloat(command_parts[0])) {
            current -> rule_type = "Float";
            current -> rule_value = command_parts[0];
            return current;               
        }

        // Check for valid variable/TypeVar/TypeName name.
        if (variables.find(command_parts[0]) != variables.end()) {
            current -> rule_type = variables[command_parts[0]];
            current -> rule_value = command_parts[0];
            return current;
        } else if (type_vars.find(command_parts[0]) != type_vars.end()) {
            current -> rule_type = "TypeVar";
            current -> rule_value = command_parts[0];
            return current;
        } else if (type_names.find(command_parts[0]) != type_names.end()) {
            current -> rule_type = "TypeName";
            current -> rule_value = command_parts[0];
            return current;
        } else if (literals.find(command_parts[0]) != literals.end()) {
            current -> rule_type = literals[command_parts[0]];
            current -> rule_value = command_parts[0];
            return current;
        }

        delete current;
        throw "ParseException: Undefined non-literal input " + command_parts[0];

    }

    return current;

}

pair<string, string> parseVarDeclare(string command, Env *env) {
    vector<string> command_parts = splitCommand(command);

    if (command_parts.size() != 2) {
        throw "ParseException: Invalid number of arguments were passed to 'declare variable' (expected 2)";
    }

    // Check that the variable name isn't taken
    if (env -> isReservedName(command_parts[0])) {
        throw "IllegalArgumentException: This name is already taken at " + command_parts[0];
    }

    // Check that the variable type is valid
    if (env -> isTypeName(command_parts[1])) {
        return {command_parts[0], command_parts[1]};
    }

    throw "IllegalArgumentException: No TypeName or TypeVar to match type " + command_parts[1]; 

}

string parseTypeVarName(string command, Env *env) {

    // Check that the variable name isn't taken
    if (env -> isReservedName(command)) {
        throw "IllegalArgumentException: This name is already taken at " + command;
    }

    return command;
}

pair<string, vector<string>> parseOpDeclare(string command, Env *env) {
    vector<string> command_parts = splitCommand(command);

    if (command_parts.size() < 2) {
        throw "ParseException: Invalid number of arguments were passed to 'declare operation' (expected 2 or more)";
    }

    // Check that the variable name isn't taken
    if (env -> isReservedName(command_parts[0])) {
        throw "IllegalArgumentException: This name is already taken at " + command_parts[0];
    }

    pair<string, vector<string>> out = pair<string,vector<string>>();
    out.first = command_parts[0];
    out.second = vector<string>();

    // Check that the variable type is valid
    for (size_t i = 1; i < command_parts.size(); i++) {
        if (!env -> isTypeName(command_parts[i])) {
            throw "IllegalArgumentException: No TypeName or TypeVar to match type " + command_parts[i];
            
        }

        out.second.push_back(command_parts[i]);
    }

    return out;

}

map<string, string> typeCheck(RuleTree *rule, Env *env, map<string, string> bound_types) {
    // If no operator, return
    if (rule -> rule_value != "") {
        return bound_types;
    }

    vector<string> cur_types = env -> operators[rule -> rule_op];
    if (cur_types.size() == 0) cur_types = default_operators[rule -> rule_op];

    // Recursive Case: Type Check subrules
    for (size_t i = 0; i < rule -> sub_rules.size(); i++) {
        RuleTree *child = rule -> sub_rules[i];
        bound_types = typeCheck(child, env, bound_types);

        string cur_type = cur_types[i + 1];

        bool is_valid = false;

        // Expected Type = Actual Type
        if (child -> rule_type == cur_type) is_valid = true;

        // Expected Type = _ (match anything)
        if (cur_type == "_") is_valid = true;

        // Type is a TypeVar.
        if (env -> type_vars.find(cur_type) != env -> type_vars.end()) {
            // If unbound, add to map. If bound, check valid.
            if (bound_types.find(cur_type) == bound_types.end()) {
                bound_types[cur_type] = child -> rule_type;
                is_valid = true;
            } else if (bound_types[cur_type] == child -> rule_type) {
                is_valid = true;
            }
        }
        
        if (!is_valid) {
            throw "IllegalArgumentException: Type matching failed.";
        }


    }

    if (bound_types.find(cur_types[0]) != bound_types.end()) {
        rule -> rule_type = bound_types[cur_types[0]];
    } else {
        rule -> rule_type = cur_types[0];
    }

    return bound_types;
}

vector<string> splitCommand(string command) {
    vector<string> command_parts = vector<string>();

    // Start of the current section of the string.
    size_t block_start = 0;
    size_t parenthesis_level = 0;

    for (size_t i = 0; i < command.length(); i++) {

        // If whitespace found, append to command_parts.
        // (Only if nonempty)
        if (command[i] == ' ' && !parenthesis_level) {

            // Check for nonempty substring.
            if (block_start < i) {

                string curr_block = command.substr(block_start, i - block_start);
                command_parts.push_back(curr_block);
            }

            block_start = i + 1;            
        }

        // Check for grouping characters.
        switch (command[i]) {
            case '(':
                // If parentheses found, check for validity (open parentheses)
                if (!parenthesis_level && block_start != i) {
                    throw "ParseException: Parentheses must be separated by whitespace in " + command;
                }

                parenthesis_level ++;
                break;
            case ')':
                // No ( to match )
                if (!parenthesis_level) {
                    throw "ParseException: No ( found to match this ) in " + command;
                }

                parenthesis_level --;
                break;

        }

    }

    // Append the final item to the list.
    if (!parenthesis_level) {

        // Check for nonempty substring.
        if (block_start < command.length()) {

            // i - 1 removes the trailing whitespace
            string curr_block = command.substr(block_start, command.length() - block_start);
            command_parts.push_back(curr_block);
        }
         
    } else {
        throw "ParseException: No ) found to match this ( in " + command;
    }

    return command_parts;
}

int charPos(string to_test, char to_find, size_t num_occurrences) {
    size_t char_ct = 0;

    for (size_t i = 0; i < to_test.length(); i++) {
        if (to_test[i] == to_find) {
            char_ct ++;
            if (char_ct == num_occurrences) return i;
        }
    }

    return -1;
}