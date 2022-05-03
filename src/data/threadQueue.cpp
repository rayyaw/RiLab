#include <pthread.h>
#include <utility>

#include "threadQueue.h"
#include "tree.h"

ThreadQueue::~ThreadQueue() {
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&q_empty);
}

void ThreadQueue::push(ProofTreeNode *node) {
    if (queue_locked) return;
    pthread_mutex_lock(&mtx);
        q.push(node);
        size++;

        pthread_cond_signal(&q_empty);
    pthread_mutex_unlock(&mtx);
}

ProofTreeNode *ThreadQueue::pop() {
    if (queue_locked) return nullptr;
    pthread_mutex_lock(&mtx);
        while (size == 0) pthread_cond_wait(&q_empty, &mtx);

        ProofTreeNode *front = q.front();
        q.pop();

        size--;

    pthread_mutex_unlock(&mtx);    

    return front;
}

void ThreadQueue::clear() {
    // FIXME: Force acquire the mutex (kick all other threads out)
    pthread_mutex_lock(&mtx);
        queue<ProofTreeNode*> newq = queue<ProofTreeNode*>();
        std::swap(q, newq);

        size = 0;
    pthread_mutex_unlock(&mtx);    
}