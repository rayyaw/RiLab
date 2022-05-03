#include <pthread.h>

#include "threadQueue.h"
#include "tree.h"

ThreadQueue::~ThreadQueue() {
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&q_empty);
}

void ThreadQueue::push(ProofTreeNode *node) {
    pthread_mutex_lock(&mtx);
        q.push(node);
        size++;
    pthread_mutex_unlock(&mtx);
}

ProofTreeNode *ThreadQueue::pop() {
    pthread_mutex_lock(&mtx);
        while (size == 0) pthread_cond_wait(&q_empty, &mtx);

        ProofTreeNode *front = q.front();
        q.pop();

        size--;

    pthread_mutex_unlock(&mtx);    

    return front;
}