#pragma once

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "data/rule.h"

using std::map;
using std::set;
using std::string;
using std::pair;
using std::vector;

// TODO
/**
 * @brief Parse a user's command into a Statement.
 * 
 * @param command The command to parse. Use () for grouping and spaces for spacing.
 * @param env The environment to modify, including all variable types, rules, etc.
 * @return string - The string to output to the console.
 */
string parseStatement(string command, Env *env);

// Helper Functions and Methods

/**
 * @brief Parse a partial rule into a tree.
 * 
 * @param command The command to parse. Use () for grouping and spaces for spacing.
 * @param env The environment to parse in.
 * @return RuleTree* The tree corresponding to that rule.
 */
RuleTree *parseRule(string command, Env *env);

/**
 * @brief Parse a variable declaration.
 * 
 * @param command The command to parse. Should be in the form [var_name] [type]
 * @param env The environment to add this to. (Only used for name availability checks.)
 * @return pair<string, string>* A pair (var name, type) that can be added to env.
 */
pair<string, string> parseVarDeclare(string command, Env *env);

/**
 * @brief Parse a TypeVar declaration.
 * 
 * @param command The new TypeVar name to parse.
 * @param env The environment holding all taken names.
 * @return string The variable name to add.
 */
string parseTypeVarName(string command, Env *env);

/**
 * @brief Parse an Op declaration.
 * 
 * @param command The command to parse. Must be of the form [op_name] [type_param_1] [...]
 * @param env  The env holding all taken names.
 * @return pair<string, vector<string>> The operation definition.
 */
pair<string, vector<string>> parseOpDeclare(string command, Env *env);

/**
 * @brief Type check a rule.
 * 
 * @param rule The rule to check.
 * @param env The env to check with.
 * @param bound_types A map from f's type vars to their assigned values in the rule.
 * (This should be set to empty when calling this function externally).
 * 
 * @return A map with updated bindings, and modifies the RuleTree to have valid types.
 * @throws IllegalArgumentException when there is a type mismatch
 */
map<string, string> typeCheck(RuleTree *rule, Env *env, map<string, string> bound_types);

/**
 * @brief Split a command into a vector of elements. Does not recurse. 
 * Will keep grouped elements (in parentheses) as a single output element.
 * 
 * @param command The command (or partial command) to split.
 * @return vector<string> The command, split on whitespace or grouping.
 */
vector<string> splitCommand(string command);

/**
 * @brief Returns the position of the nth to_find character in the string.
 * (The function is 1-indexed.)
 * 
 * @param to_test The string to search.
 * @param to_find The character to find.
 * @param num_occurrences Find the nth occurrence of the character.
 * @return size_t The position of that character in the string, 
 * or -1 if the character wasn't found.
 */
int charPos(string to_test, char to_find, size_t num_occurrences);

