#pragma once

#include <pthread.h>
#include <queue>

#include "tree.h"

using std::queue;

static bool queue_locked = false;

// Producer-Consumer Queue with no max size. Thread safe.
class ThreadQueue {
    public:
    ThreadQueue() = default;
    ~ThreadQueue();

    void push(ProofTreeNode *node);
    ProofTreeNode *pop();

    void clear();

    private:
    queue<ProofTreeNode*> q = queue<ProofTreeNode*>();

    size_t size = 0;

    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t q_empty = PTHREAD_COND_INITIALIZER;
};