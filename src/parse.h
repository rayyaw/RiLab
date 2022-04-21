#pragma once

#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "rule.h"

using std::map;
using std::set;
using std::string;
using std::pair;
using std::vector;

// Valid types of statements.
enum StatementType {
    ASK,

    DECLARE_RULE,
    DECLARE_FUNC,
    DECLARE_TYPE,

    DECLARE_TYPEVAR,
    DECLARE_VAR,

    UNKNOWN
};

/**
 * @brief Struct containing a single statement (user input).
 * Contains the type of the statement as well as its value.
 */
struct Statement {
    StatementType type;
    void *value;
};

/**
 * @brief Parse a user's command into a Statement.
 * 
 * @param command The command to parse. Use () for grouping and spaces for spacing.
 * @param env The environment to modify, including all variable types, rules, etc.
 * @return Statement* A statement corresponding to that rule.
 */
Statement *parseStatement(string command, Env env);

// Helper Functions and Methods

// FIXME: Add operator type and arg-count-checking
// FIXME: Allow variables to have a TypeVar type
/**
 * @brief Parse a partial rule into a tree.
 * 
 * @param command The command to parse. Use () for grouping and spaces for spacing.
 * @param variables Set of valid variable names.
 * @param type_vars Set of valid type_var names.
 * @param op_names Set of valid operation names.
 * @param type_names Set of valid type names.
 * @return RuleTree* The tree corresponding to that rule.
 */
RuleTree *parseRule(string command, 
    map<string, string> variables, set<string> type_vars, 
    set<string> op_names, set<string> type_names);

// TODO + check not ANYWHERE in env for these functions (or in reserved words/operators)
// reserved words = ask, declare, rule, var, typevar, typename
/**
 * @brief Parse a variable declaration.
 * 
 * @param command The command to parse. Should be in the form [var_name] [type]
 * @param env The environment to add this to. (Only used for name availability checks.)
 * @return pair<string, string>* A pair (var name, type) that can be added to env.
 */
pair<string, string> parseVarDeclare(string command, Env *env);

// TODO
/**
 * @brief Parse a TypeVar declaration.
 * 
 * @param command The new TypeVar name to parse.
 * @param env The environment holding all taken names.
 * @return string The variable name to add.
 */
string parseTypeVarDeclare(string command, Env *env);

// TODO
/**
 * @brief Parse an Op declaration.
 * 
 * @param command The command to parse. Must be of the form [op_name] [type_param_1] [...]
 * @param env  The env holding all taken names.
 * @return pair<string, vector<string>> The operation definition.
 */
pair<string, vector<string>> parseOpDeclare(string command, Env *env);

/**
 * @brief Split a command into a vector of elements. Does not recurse. 
 * Will keep grouped elements (in parentheses) as a single output element.
 * 
 * @param command The command (or partial command) to split.
 * @return vector<string> The command, split on whitespace or grouping.
 */
vector<string> splitCommand(string command);



