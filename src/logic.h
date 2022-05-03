#pragma once

#include <pthread.h>
#include <queue>
#include <vector>

#include "data/rule.h"
#include "data/threadQueue.h"
#include "data/tree.h"

using std::queue;

extern bool stop_ask;

/**
 * @brief Run an Ask query in the given environment
 * 
 * @param env The environment to run on. env -> ask_rule should be the rule to run.
 * @param tasks The queue for sending tasks to threads
 * @param results The queue for getting results from threads
 * @return a string with the proof if the statement is PROVABLY true, or "Could not prove X" otherwise.
 */
string runAsk(Env *env, ThreadQueue *tasks, ThreadQueue *results);

/**
 * @brief Partial Run Ask only used in threads.
 * 
 * @param env The environment to run on. env -> ask_rule should be the rule to run.
 * @param recursion_limit The max recursion depth to go to.
 * @param root The proof tree node to start from.
 * @return The leaf node (in the same tree) that reached a tautology.
 */
ProofTreeNode *runAskWorker(Env *env, size_t recursion_limit, ProofTreeNode *root);

/**
 * @brief Apply one rule to another.
 * 
 * @param env The environment to run in.
 * @param apply The rule to be applied. 
 * @param victim The rule to be applied to.
 * @param side Which side of the --<> to expand.
 * @return RuleTree The new, transformed rules.
 * @throws -1 if the rules are incompatible. 
 */
RuleTree* applyRule(Env *env, RuleTree *apply, RuleTree *victim, bool side);

/**
 * @brief Expand a Proof Tree Node by generating its children (which may or may not be valid)
 * 
 * @param node The node to expand
 * @param env The env containing the rules to use
 * @return None, modifies the node in place.
 */
void expandNode(ProofTreeNode *node, Env *env);

/**
 * @brief Show the full proof as a string.
 * @param leaf The leaf node containing two equal expressions.
 * @return The proof. Backtracks up the tree to show every step. 
 */
string showProof(ProofTreeNode *leaf);

/**
 * @brief Perform the specified substitutions, then return a deep copy of the rule.
 * 
 * @param env The enviroment to run in.
 * @param original The original rule.
 * @param substitutions The list of substitutions. The types are assumed to match
 * @return RuleTree* A copy of the rule, with the substitutions made
 * Example: (Var v).subs (v -> (Var s)) = (Var s)
 */
RuleTree *substitute(Env *env, RuleTree *original, map<string, RuleTree*> substitutions);

/**
 * @brief Checks if one rule generalizes to another.
 * 
 * @param env The environment to run in.
 * @param general The more general rule.
 * @param substitutions The substitutions made so far.
 * @return map<string, RuleTree*> The new map of substitutions.
 * @throws an error (string) if no generalization is possible.
 */
map<string, RuleTree*> generalize(Env *env, RuleTree *general, RuleTree *specific, map<string, RuleTree*> substitutions);