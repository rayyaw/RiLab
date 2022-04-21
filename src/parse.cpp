#include "globals.h"
#include "parse.h"
#include "rule.h"
#include "utils.h"

#include <map>
#include <set>
#include <string>
#include <vector>

using std::map;
using std::set;
using std::string;
using std::vector;

using namespace rules;

size_t rule_id = 0;

RuleTree *parseRule(string command, 
    map<string, string> variables, set<string> type_vars, 
    map<string, vector<string>> op_names, set<string> type_names) {

    vector<string> command_parts = splitCommand(command);

    if (command_parts.empty()) {
        throw "ParseException: Empty input is invalid";
    }

    RuleTree *current = new RuleTree();
    current -> rule_id = rule_id ++;
    current -> rule_value = "";
    current -> rule_op = "";

    // Assume a rule is of the form (op a b ...) like how Haskell functions are applied

    // If this is an operator, split and recurse.
    if (default_operators.find(command_parts[0]) != default_operators.end() ||
    op_names.find(command_parts[0]) != op_names.end()) {
        current -> rule_op = command_parts[0];

        current -> sub_rules = vector<RuleTree*>();

        vector<string> op_params;
        if (default_operators.find(command_parts[0]) != default_operators.end()) {
            op_params = default_operators[command_parts[0]];
        } else {
            op_params = op_names[command_parts[0]];
        } 

        // Check that the function argument count is correct
        if (op_params.size() != command_parts.size()) {
            throw "ParseException: Invalid number of arguments passed to function " + command_parts[0];
        }

        current -> rule_type = op_params[0];

        for (size_t i = 1; i < command_parts.size(); i++) {
            string next_command = command_parts[i];

            // Remove grouping if needed
            if (next_command[0] == '(') {
                next_command = next_command.substr(1, next_command.length() - 2);
            }

            current -> sub_rules.push_back(
                parseRule(next_command, variables, type_vars, op_names, type_names)
            );

            string cur_type = current -> sub_rules[i - 1] -> rule_type;

            // Type check the parameters. _ is a wildcard that matches anything.
            if (cur_type != op_params[i] && op_params[i] != "_") {
                throw "ParseException: Type mismatch in operation at " + cur_type;
            }
        }
    }

    // If no operator was found, check this is a variable of some kind and store it.
    else {
        // Check that there's only a single extra command_part, with no grouping.
        if (command_parts.size() != 1) {
            throw "ParseException: Single values must be a single value at " + command;
        } else if (command_parts[0][0] == '(') {
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
        }

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
    if (command_parts[1] == "_" || command_parts[1] == "Int" || 
        command_parts[1] == "Bool" || command_parts[1] == "Float" ||
    env -> type_vars.find(command_parts[1]) != env -> type_vars.end() ||
    env -> type_names.find(command_parts[1]) != env -> type_names.end()) {
        return {command_parts[0], command_parts[1]};
    }

    throw "IllegalArgumentException: No TypeName or TypeVar to match type " + command_parts[1]; 

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