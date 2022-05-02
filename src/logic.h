#pragma once

#include <pthread.h>
#include <queue>
#include <vector>

#include "data/rule.h"
#include "data/tree.h"

using std::queue;

// FIXME
struct Task {
    void *unused;
};

struct Result {
    void *unused;
};

// Info on the threads in use
struct ThreadInfo {
    size_t num_threads;

    // I/O Queues
    queue<Task> *task_q;
    queue<Result> *result_q;

    // Mutex for I/O Queues
    pthread_mutex_t *task_mutex;
    pthread_mutex_t *result_mutex;

    // Condition Variables for I/O Queues
    pthread_cond_t *task_cv;
    pthread_cond_t *result_cv;
};

// FIXME make fully multithreaded
/**
 * @brief Run an Ask query in the given environment
 * 
 * @param env The environment to run on. env -> ask_rule should be the rule to run.
 * @param recursion_limit The maximum recursion depth to go to.
 * @param threads Thread information to use.
 * @return a string with the proof if the statement is PROVABLY true, or "Could not prove X" otherwise.
 */
string runAsk (Env *env, size_t recursion_limit, ThreadInfo threads);

/**
 * @brief Partial Run Ask only used in threads.
 * 
 * @param env The environment to run on. env -> ask_rule should be the rule to run.
 * @param task Information on the task to run.
 * @return a result corresponding to the proof, 
 * or interrupted if another thread finishes a proof first.
 */
Result runAskWorker (Env *env, const Task &task);

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