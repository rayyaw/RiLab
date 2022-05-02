//#include <pthread.h>
#include <queue>
#include <string>
#include <vector>

#include "data/rule.h"
#include "data/tree.h"
#include "logic.h"

using std::queue;
using std::string;
using std::vector;

string runAsk(Env *env, size_t recursion_limit, ThreadInfo threads) {
    // Initialize root rule
    RuleTree *ask = env -> ask_rule;
    
    queue<ProofTreeNode*> next_rules;

    ProofTreeNode *tree_root = new ProofTreeNode();

    tree_root -> to_prove_remainder = ask;
    tree_root -> applied_rule = nullptr;

    expandNode(tree_root, env);

    size_t states_to_expand = tree_root -> children.size();
    size_t next_layer_states = 0;

    for (ProofTreeNode *node : tree_root -> children) {
        next_rules.push(node);
    }

    // Use standard BFS with a recursion limit
    while (!next_rules.empty()) {
        ProofTreeNode *current = next_rules.front();
        next_rules.pop();

        // Check if the rule (or its substitution) is equal (up to varsubs) to an existing rule. 
        // If so, print the proof and return.
        for (RuleTree *rule : env -> rules) {
            try {
                generalize(env, rule, current -> to_prove_remainder, map<string, RuleTree*>());
                return showProof(current);
            } catch (const char &e) {
                // Do nothing, this is a dead end
            }
        }

        for (RuleTree *rule : env -> rules) {
            try {
                generalize(env, rule, current -> to_prove_remainder, map<string, RuleTree*>());
                return showProof(current);
            } catch (const char &e) {
                // Do nothing, this is a dead end
            }
        }

        // Apply both directions
        try {
            // Apply then expand and push back
            RuleTree *new_rule = applyRule(env, current -> applied_rule, current -> to_prove_remainder, true);
            current -> to_prove_remainder = new_rule;

            expandNode(current, env);
            next_layer_states += current -> children.size();

        } catch (char const &e) {
            // Could not apply the rule, so continue
        }

        try {
            // Apply then expand and push back
            RuleTree *new_rule = applyRule(env, current -> applied_rule, current -> to_prove_remainder, false);
            current -> to_prove_remainder = new_rule;

            expandNode(current, env);
            next_layer_states += current -> children.size();

        } catch (char const &e) {
            // Could not apply the rule, so continue
        }

        // Check if we've reached the recursion limit
        states_to_expand --;

        if (states_to_expand == 0) {
            recursion_limit --;
            states_to_expand = next_layer_states;
            next_layer_states = 0;
        }

        if (recursion_limit == 0) {
            break;
        }
    }

    return "Was unable to prove the rule"; 

}

void expandNode(ProofTreeNode *node, Env *env) {
    for (RuleTree *rule : env -> rules) {
        ProofTreeNode *child = new ProofTreeNode();
        child -> parent = node;
        child -> to_prove_remainder = node -> to_prove_remainder;
        child -> applied_rule = rule;

        node -> children.insert(child);
    }
}

/** VALID RULES TO APPLY:
 * We will use the term "valid rule" to mean any rule in env -> rules 
 * (ie, any axiom or already proven theorem)
 * 
 * We will use the term "accepted" to mean any rule that is correct, and forms
 * an intermediate step of the proof.
 * 
 * if A is valid then return as it's also accepted. Add it to the env (with a mutex).
 * 
 * if A and (--> B A) is valid then B is accepted.
 * if A and (--<> A B) is valid then B is accepted.
 * if A and (--<> B A) is valid then B is accepted.
 * 
 * if B and (|A (cond) B) is valid then cond is accepted.
 * 
 * Notes
 * You must keep track and allow variable substitutions, both for Var and TypeVar.
 * (This is done via equivalent.)
 * Must apply to children as well
 * 
 */

RuleTree *applyRule(Env *env, RuleTree *apply, RuleTree *victim, bool direction) {
    // Check for valid operators
    if (apply -> rule_op == "-->" && direction) {
        // If this throws an error just go up to the next exec level
        map<string, RuleTree*> subs = generalize(env, apply -> sub_rules[1], victim, map<string, RuleTree*>());
        return substitute(env, apply -> sub_rules[0],  subs);
    } else if (apply -> rule_op == "--<>" && direction) {
        map<string, RuleTree*> subs = generalize(env, apply -> sub_rules[1], victim, map<string, RuleTree*>());
        return substitute(env, apply -> sub_rules[0],  subs);
    }

    else if (apply -> rule_op == "--<>") {
        map<string, RuleTree*> subs = generalize(env, apply -> sub_rules[0], victim, map<string, RuleTree*>());
        return substitute(env, apply -> sub_rules[1],  subs);
    }

    // Rule could not be applied, so throw an error
    throw "GeneralizeError: Can only apply rules with --> or --<> (or incorrect direction)";
}

// FIXME
string showProof(ProofTreeNode *leaf) {
    throw "Unimplemented!";
}

RuleTree *substitute(Env *env, RuleTree *original, map<string, RuleTree*> substitutions) {
    // Base Case
    if (substitutions.find(original -> rule_value) != substitutions.end()) {
        return new RuleTree(*substitutions[original -> rule_value]);
    }

    // Make a copy and recursively substitute variables
    RuleTree *copy = new RuleTree();
    copy -> rule_op = original -> rule_op;
    copy -> rule_type = original -> rule_type;
    copy -> rule_value = original -> rule_value;

    // Perform type var substitutions
    if (env -> type_var_subs.find(original -> rule_type) != env -> type_var_subs.end()) {
        copy -> rule_type = env -> type_var_subs[original -> rule_type];
    }

    // Recursive Case
    for (RuleTree *child : original -> sub_rules) {
        copy -> sub_rules.push_back(substitute(env, child, substitutions));
    }

    return copy;

}

map<string, RuleTree*> generalize(Env *env, RuleTree *general, RuleTree *specific, 
    map<string, RuleTree*> substitutions) {
    if (general -> rule_op != specific -> rule_op) {
        throw "GeneralizeError: Could not match the operators.";
    }

    RuleTree *type_fixed = new RuleTree(*general);

    // Check for wild card (_) substitutions, which are free
    if (general -> rule_type == "_") {
        type_fixed -> rule_type = general -> rule_type;
    }

    // Check if the TypeVar needs to be substituted (general)
    else if (env -> type_var_subs.find(general -> rule_op) != env -> type_var_subs.end()) {
        type_fixed -> rule_type = env -> type_var_subs[general -> rule_op];
    }

    // Check for general being a TypeVar (no substitution so far)
    else if (env -> type_vars.find(general -> rule_type) != env -> type_vars.end()) {
        type_fixed -> rule_type = specific -> rule_type;
        env -> type_var_subs[type_fixed -> rule_type] = specific -> rule_type;
    }

    map<string, RuleTree*> new_subs = substitutions;

    // Base Case
    if (type_fixed -> rule_value != "") {
        // Checking for literal in type_fixed (requires EXACT value)
        if (env -> literals.find(type_fixed -> rule_value) != env -> literals.end()) {
            if (type_fixed -> rule_value == specific -> rule_value) {
                delete type_fixed;
                return new_subs;
            } else {
                delete type_fixed;
                throw "GeneralizeError: Two literals are not equal";
            }
        }

        // Checking for variable in type_fixed (requires a substitution)
        // Substitution already made -> check validity
        else if (new_subs.find(type_fixed -> rule_value) != new_subs.end()) {
            if (new_subs[type_fixed -> rule_value] == specific) {
                delete type_fixed;
                return new_subs;
            } else {
                delete type_fixed;
                throw "GeneralizeError: Two variables are incompatible";                
            }            
        } else { // No substitution present -> add one
            new_subs[type_fixed -> rule_value] = specific;
            delete type_fixed;
            return new_subs;
        }

    }

    // Bind children and recurse
    if (type_fixed -> rule_type == specific -> rule_type) {
        // Since the rules are already type checked,
        // they're guaranteed to have the same length
        for (size_t i = 0; i < general -> sub_rules.size(); i++) {
            new_subs = generalize(env, type_fixed -> sub_rules[i], specific -> sub_rules[i], new_subs);
        }

        delete type_fixed;
        return new_subs;
        
    }

    delete type_fixed;
    throw "GeneralizeError: Could not match rule output types.";

}