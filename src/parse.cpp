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

// Note: _ is the default TypeVar. It will match against anything.
namespace rules {
    map<string, vector<string>> default_operators = {
        {"-->", {"_", "_"}},  // Implication
        {"--<>", {"_", "_"}}, // Equivalence
        
        {"!", {"_"     }}, // NOT
        {"&", {"_", "_"}}, // AND
        {"|", {"_", "_"}}, // OR

        {":", {"_", "_"}},  // such that
        {"|A", {"_"    }}, // for all
        {"|E", {"_"    }}, // there exists

        {"in", {"_", "_"}}  // Set/Type Inclusion
    };
}

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

        // FIXME - type and arg-count checking
        current -> sub_rules = vector<RuleTree*>();

        for (size_t i = 1; i < command_parts.size(); i++) {
            string next_command = command_parts[i];

            // Remove grouping if needed
            if (next_command[0] == '(') {
                next_command = next_command.substr(1, next_command.length() - 2);
            }

            current -> sub_rules.push_back(
                parseRule(next_command, variables, type_vars, op_names, type_names)
            );
        }
    }

    // If no operator was found, check this is a variable of some kind and store it.
    else {
        // Check that there's only a single extra command_part, with no grouping.
        if (command_parts.size() != 1) {
            throw "ParseException: Single values must be a single value";
        } else if (command_parts[0][0] == '(') {
            throw "ParseException: Single values may not contain grouping";
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

        throw "ParseException: Undefined non-literal input";

    }

    return current;

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
                    throw "ParseException: Parentheses must be separated by whitespace";
                }

                parenthesis_level ++;
                break;
            case ')':
                // No ( to match )
                if (!parenthesis_level) {
                    throw "ParseException: No ( found to match this )";
                }

                parenthesis_level --;
                break;

        }

    }

    // Append the final item to the list.
    if (!parenthesis_level) {

        // Check for nonempty substring.
        if (block_start + 1 < command.length()) {

            // i - 1 removes the trailing whitespace
            string curr_block = command.substr(block_start, command.length() - block_start);
            command_parts.push_back(curr_block);
        }
         
    } else {
        throw "ParseException: No ) found to match this (";
    }

    return command_parts;
}