#pragma once

#include <pthread.h>
#include <semaphore.h>
#include <string>

using std::string;

// Check if the given string can be converted to an int or float.
bool isInt(string v);
bool isFloat(string v);

// Thread Safe Producer-Consumer Queue.
template <typename T>
class TaskQueue {
    public:
    TaskQueue(size_t max_size);
    ~TaskQueue();

    void push(T task);
    T pop();

    private:
    pthread_mutex_t *m;
    sem_t *empty_spaces;
    sem_t *full_spaces;

    size_t size;
    size_t capacity;

    T *queue;
    size_t cur_pos;

};