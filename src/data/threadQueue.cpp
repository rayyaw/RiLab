#include <iostream>
#include <pthread.h>
#include <utility>

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

        pthread_cond_broadcast(&q_empty);
    pthread_mutex_unlock(&mtx);
}

ProofTreeNode *ThreadQueue::pop() {
    pthread_mutex_lock(&mtx);
        while (size == 0 && !locked) pthread_cond_wait(&q_empty, &mtx);
        if (locked) {
            pthread_mutex_unlock(&mtx);
            return nullptr;
        }

        ProofTreeNode *front = q.front();
        q.pop();

        size--;

    pthread_mutex_unlock(&mtx);    

    return front;
}

void ThreadQueue::lock() {
    clear();
    pthread_mutex_lock(&mtx);
        locked = true;
        pthread_cond_broadcast(&q_empty);
    pthread_mutex_unlock(&mtx);
}

void ThreadQueue::unlock() {
    pthread_mutex_lock(&mtx);
        locked = false;
    pthread_mutex_unlock(&mtx);
}

void ThreadQueue::clear() {
    pthread_mutex_lock(&mtx);
        queue<ProofTreeNode*> newq = queue<ProofTreeNode*>();
        std::swap(q, newq);

        size = 0;
    pthread_mutex_unlock(&mtx);    
}