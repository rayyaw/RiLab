#include <iostream>
#include <map>
#include <pthread.h>
#include <queue>
#include <signal.h>
#include <string>
#include <set>
#include <vector>

#include "../src/data/rule.h"
#include "../src/data/tree.h"
#include "../src/logic.h"
#include "../src/parse.h"

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::getline;
using std::istream;
using std::map;
using std::queue;
using std::string;
using std::set;
using std::vector;

// Global variables
static size_t recursion_limit;

static Env *env;

static ThreadQueue *tasks;
static ThreadQueue *results;

extern bool queue_locked;
extern bool stop_ask;

// FIXME: write README
// FIXME: test with valgrind + control-C proof stopping
void handleSigint(int sig) {
    // Force children to stop and return to runWorker.
    stop_ask = true;

    // Force the main thread to stop adding tasks (but don't stop runAsk).
    // This also forces children to stop adding results.
    queue_locked = true;

    // Queues are emptied after runAsk returns to main
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

    pthread_exit(NULL);
}

// Get a command from the given istream.
void handleIOCommand(istream &in, Env *env) {
    cout << ">>> ";
    string command;

    getline(in, command);

    try {
        string output = parseStatement(command, env);
        cout << output << endl;
    }

    catch (char const *e) {
        cerr << e << endl;
    }

}

int main(int argc, char *argv[]) {
    
    if (argc > 3) {
        cerr << "Usage: rilab [num_threads] [recursion_limit]" << endl;
        return -1;
    }

    env = new Env();
    tasks = new ThreadQueue();
    results = new ThreadQueue();

    // Setting ask parameters.
    size_t num_threads;
    if (argc >= 2) num_threads = atoi(argv[1]);
    else num_threads = 4;
    if (num_threads <= 0 || num_threads >= 255) num_threads = 4;

    if (argc >= 3) recursion_limit = atoi(argv[2]);
    else recursion_limit = 10;

    // Ignore SIGINT in the main thread and force children to stop.
    // SIGINT should only force the children to stop (in runAsk)
    signal(SIGINT, handleSigint);

    // Start threads.
    pthread_t *threads = new pthread_t[num_threads];

    for (int i = 0; i < num_threads; i++) {
        pthread_create(threads + i, NULL, runWorker, NULL);
    }

    // Print basic info about RiLab.
    cout << "RiLab Theorem Proving Software." << endl;
    cout << "Made by RileyTech (https://github.com/rayyaw/RiLab)." << endl << endl;
    cout << "Licensing: The executable file is licensed under CC Attribution No-Derivatives 4.0. You may use this for any purpose (including commercial and military/research uses, etc.) as long as you cite the source. No modification is allowed." << endl << endl;
    cout << "Licensing: The source code for RiLab is licensed under CC Attribution SA NC 4.0. You may use, modify, or adapt this code for noncommercial purposes, as long as you share alike and cite the author." << endl << endl;

    // Main loop.
    while (!cin.eof()) {
        // Get user input.
        handleIOCommand(cin, env);

        if (env -> ask_rule) {
            try {
                queue_locked = false;
                stop_ask = false;

                cout << runAsk(env, tasks, results);
            } catch (char const *e) {
                cerr << e << endl;
            }

            // Empty the queues to avoid stale data
            tasks -> clear();
            results -> clear();
        }
    }

    // FIXME - cleanup threads / fix memory leaks
    delete[] threads;
    delete env;
    return 0;

}