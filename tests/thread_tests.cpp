#include "catch.hpp"

#include "../src/data/rule.h"
#include "../src/data/threadQueue.h"
#include "../src/data/tree.h"
#include "../src/logic.h"
#include "../src/parse.h"

#include <iostream>
#include <map>
#include <pthread.h>
#include <queue>
#include <set>
#include <signal.h>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using std::cerr;
using std::cout;
using std::endl;
using std::map;
using std::ostringstream;
using std::pair;
using std::queue;
using std::set;
using std::string;
using std::vector;

static Env *env;

static size_t recursion_limit;

static ThreadQueue *tasks;
static ThreadQueue *results;

extern bool queue_locked;
extern bool stop_ask;

void handleSigint(int sig) {
    stop_ask = true;
    queue_locked = true;
}

void *runWorker (void *unused) {
    // This will stop when main returns, so no need to have a variable to force exit.
    while (true) {
        // Get a new task to run.
        ProofTreeNode *node = tasks -> pop();

        try {
            ProofTreeNode *result = runAskWorker(env, recursion_limit, node);
            results -> push(result);
        } catch (char const *e) {
            results -> push(nullptr);
        }
        
    }

}

void setupTest() {
    env = new Env();
    parseStatement("source tests/nat.rilab", env);

    tasks = new ThreadQueue();
    results = new ThreadQueue();

    recursion_limit = 5;

    signal(SIGINT, handleSigint);

}

TEST_CASE("Simple runAsk for multithread") {
    setupTest();

    pthread_t worker;
    pthread_create(&worker, NULL, runWorker, NULL);

    env -> ask_rule = parseRule("InNatural Two", env);
    string proof = runAsk(env, tasks, results);

    REQUIRE(proof == "==> (InNatural (Natural Zero))\nApply rule (--> (InNatural (Natural x)) (InNatural (S (Natural x))))\n\n==> (InNatural (S (Natural Zero)))\nApply rule (--> (InNatural (Natural x)) (InNatural (S (Natural x))))\n\n==> (InNatural (S (S (Natural Zero))))\nApply rule (--<> (InNatural (Natural Two)) (InNatural (S (S (Natural Zero)))))\n\n");

}

TEST_CASE("Simple runAsk for multithread (parseStatement)") {
    setupTest();

    pthread_t worker;
    pthread_create(&worker, NULL, runWorker, NULL);

    parseStatement("ask InNatural Two", env);
    string proof = runAsk(env, tasks, results);

    REQUIRE(proof == "==> (InNatural (Natural Zero))\nApply rule (--> (InNatural (Natural x)) (InNatural (S (Natural x))))\n\n==> (InNatural (S (Natural Zero)))\nApply rule (--> (InNatural (Natural x)) (InNatural (S (Natural x))))\n\n==> (InNatural (S (S (Natural Zero))))\nApply rule (--<> (InNatural (Natural Two)) (InNatural (S (S (Natural Zero)))))\n\n");

}