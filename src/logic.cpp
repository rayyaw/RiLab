#include <iostream>
#include <pthread.h>
#include <queue>
#include <signal.h>
#include <sstream>
#include <string>
#include <vector>

#include "data/threadQueue.h"
#include "data/rule.h"
#include "data/tree.h"
#include "logic.h"

using std::endl;
using std::ostringstream;
using std::queue;
using std::string;
using std::vector;

string runAsk(Env *env, ThreadQueue *tasks, ThreadQueue *results) {
    // Initialize root rule
    RuleTree *ask = env -> ask_rule;

    if (ask == nullptr) throw "Invalid Ask query";

    ProofTreeNode *tree_root = new ProofTreeNode();

    tree_root -> to_prove_remainder = ask;
    tree_root -> applied_rule = nullptr;

    size_t task_count = 0;

    expandNode(tree_root, env);
    for (ProofTreeNode *child : tree_root -> children) {
        tasks -> push(child);
        task_count++;
    }

    // Merge from threads and output the final result
    for (size_t i = 0; i < task_count; i++) {
        ProofTreeNode *node = results -> pop();

        if (node != nullptr) {
            string proof = showProof(node);

            // Send SIGINT to stop the other threads and empty the queues
            raise(SIGINT);

            delete tree_root;
            return proof;
        }
    }

    delete tree_root;
    throw "RecursionLimitReached: Was unable to prove the rule"; 

}

ProofTreeNode *runAskWorker(Env *env, size_t recursion_limit, ProofTreeNode *root) {
    // Initialize root rule    
    queue<ProofTreeNode*> next_rules;
    next_rules.push(root);

    size_t states_to_expand = 1;
    size_t next_layer_states = 0;

    // Use standard BFS with a recursion limit
    while (!next_rules.empty()) {
        ProofTreeNode *current = next_rules.front();
        next_rules.pop();

        // Check if the rule (or its substitution) is equal (up to varsubs) to an existing rule. 
        // If so, print the proof and return.
        for (RuleTree *rule : env -> rules) {
            try {
                generalize(env, rule, current -> to_prove_remainder, map<string, RuleTree*>());

                if (stop_ask) throw "SIGINT: User interrupt received";
                else return current;
            } catch (char const *e) {
                if (stop_ask) throw "SIGINT: User interrupt received";
            }
        }

        // Apply both directions
        try {
            // Apply then expand and push back
            RuleTree *new_rule = applyRule(env, current -> applied_rule, current -> to_prove_remainder, true);
            delete current -> to_prove_remainder;
            current -> to_prove_remainder = new_rule;

            expandNode(current, env);
            next_layer_states += current -> children.size();
            
            for (ProofTreeNode *child : current -> children) next_rules.push(child);

        } catch (char const *e) {
            // Could not apply the rule, so continue
        }

        try {
            // Apply then expand and push back
            RuleTree *new_rule = applyRule(env, current -> applied_rule, current -> to_prove_remainder, false);
            delete current -> to_prove_remainder;
            current -> to_prove_remainder = new_rule;

            expandNode(current, env);
            next_layer_states += current -> children.size();

            for (ProofTreeNode *child : current -> children) next_rules.push(child);

        } catch (char const *e) {
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

    throw "RecursionLimitReached: Was unable to prove the rule";

}

void expandNode(ProofTreeNode *node, Env *env) {
    for (RuleTree *rule : env -> rules) {
        ProofTreeNode *child = new ProofTreeNode();
        child -> parent = node;
        child -> to_prove_remainder = new RuleTree(*node -> to_prove_remainder);
        child -> applied_rule = new RuleTree(*rule);

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

string showProof(ProofTreeNode *leaf) {
    ostringstream output;

    // The two children nodes are duplicates (recurse down and check for tautology)
    ProofTreeNode *current = leaf -> parent;
    
    // Recurse back up the tree.
    while (current != nullptr && current -> applied_rule != nullptr) {
        output << "==> " << *current -> to_prove_remainder << endl;
        output << "Apply rule " << *current -> applied_rule << endl;
        output << endl;
        current = current -> parent;
    }

    return output.str();
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

    string rule_type = general -> rule_type;

    // Check for wild card (_) substitutions, which only require binding variables
    if (general -> rule_type == "_") {
        rule_type = specific -> rule_type;
    }

    // Check if the TypeVar needs to be substituted (general)
    else if (env -> type_var_subs.find(general -> rule_type) != env -> type_var_subs.end()) {
        rule_type = env -> type_var_subs[general -> rule_type];
    }

    // Check for general being a TypeVar (no substitution so far)
    else if (env -> type_vars.find(general -> rule_type) != env -> type_vars.end()) {
        rule_type = specific -> rule_type;
        env -> type_var_subs[general -> rule_type] = specific -> rule_type;
    }

    map<string, RuleTree*> new_subs = substitutions;

    // Check for type equality (up to generalizations)
    if (rule_type != specific -> rule_type) {
        throw "GeneralizeError: Could not match rule output types.";
    }

    // Base Case
    if (general -> rule_value != "") {
        // Checking for literal in type_fixed (requires EXACT value)
        if (env -> literals.find(general -> rule_value) != env -> literals.end()) {
            if (general -> rule_value == specific -> rule_value) {
                return new_subs;
            } else {
                throw "GeneralizeError: Two literals are not equal";
            }
        }

        // Checking for variable in type_fixed (requires a substitution)
        // Substitution already made -> check validity
        else if (new_subs.find(general -> rule_value) != new_subs.end()) {
            if (new_subs[general -> rule_value] == specific) {
                return new_subs;
            } else {
                throw "GeneralizeError: Two variables are incompatible";                
            }            
        } else { // No substitution present -> add one
            new_subs[general -> rule_value] = specific;
            return new_subs;
        }

    }

    // Bind children and recurse. If recursive case, operators must match.
    if (general -> rule_op != specific -> rule_op) {
        throw "GeneralizeError: Could not match the operators.";
    }
    
    // Since the rules are already type checked,
    // they're guaranteed to have the same length
    for (size_t i = 0; i < general -> sub_rules.size(); i++) {
        new_subs = generalize(env, general -> sub_rules[i], specific -> sub_rules[i], new_subs);
    }

    return new_subs;

}