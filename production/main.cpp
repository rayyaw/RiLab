#include <iostream>
#include <map>
#include <pthread.h>
#include <queue>
#include <string>
#include <set>
#include <vector>

#include "../src/data/rule.h"
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
#define RECURSION_LIMIT 100

static volatile Env *env;
// UNCOMMENT WHEN USING MULTITHREADING --- FIXME
/*
static volatile queue<Task> tasks;
static volatile queue<Result> results;

pthread_mutex_t task_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t result_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t task_cv = PTHREAD_COND_INITIALIZER;
pthread_cond_t result_cv = PTHREAD_COND_INITIALIZER;

void *runWorker (void *unused) {
    // This will stop when main returns, so no need to have a variable to force exit.
    while (true) {
        // Get a new task to run.
        pthread_mutex_lock(&task_mutex);
            while (tasks.empty()) {
                pthread_cond_wait(&task_cv, &task_mutex);
            }

            Task task = tasks.front();
            tasks.pop();
        pthread_mutex_unlock(&task_mutex);

        Result result = runAskWorker(env, task);

        // Return the results of the current task.
        pthread_mutex_lock(&result_mutex);
            results.push(result);
            pthread_cond_signal(&result_cv);
        pthread_mutex_unlock(&result_mutex);

    }

    return nullptr;
}
*/

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
    
    if (argc != 2) {
        cout << "Usage: rilab <num_threads>" << endl;
        return -1;
    }

    env = new env();

    int num_threads = atoi(argv[1]);
    if (num_threads <= 0 || num_threads >= 255) num_threads = 10;

    // Start threads.
    pthread_t *threads = new pthread_t[num_threads];

    for (int i = 0; i < num_threads; i++) {
        pthread_create(threads + i, NULL, runWorker, NULL);
    }

    // Thread info to send for runAsk.
    ThreadInfo threads = ThreadInfo();
    threads.num_threads = num_threads;
    threads.task_q = &tasks;
    threads.result_q = &results;

    threads.task_mutex = &task_mutex;
    threads.result_mutex = &result_mutex;

    threads.task_cv = &task_cv;
    threads.result_cv = &result_cv;

    // Print basic info about RiLab.
    cout << "RiLab Theorem Proving Software." << endl;
    cout << "Made by RileyTech (https://github.com/rayyaw/RiLab)." << endl << endl;
    cout << "Licensing: The executable file is licensed under CC Attribution No-Derivatives 4.0. You may use this for any purpose (including commercial and military/research uses, etc.) as long as you cite the source. No modification is allowed." << endl << endl;
    cout << "Licensing: The source code for RiLab is licensed under CC Attribution SA NC 4.0. You may use, modify, or adapt this code for noncommercial purposes, as long as you share alike and cite the author." << endl << endl;

    // Main loop.
    while (!cin.eof()) {
        // Get user input.
        handleIOCommand(cin, env);

        // FIXME: Add signal handler on control+C to break out of this. (use sigaction)
        if (env -> ask_rule) {
            cout << runAsk(env, RECURSION_LIMIT, threads);
        }
    }

    delete env;
    return 0;

}