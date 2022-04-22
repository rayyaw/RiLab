#include <pthread.h>
#include <semaphore.h>
#include <string>

#include "utils.h"

using std::string;

// Taken from https://linuxhint.com/check-integer-input-cpp/. Checks if a string is an integer.
bool isInt(string v) {
    for (int i = 0; i < v.length(); i++) {
        if (!isdigit(v[i])) return false;
    }
        
    return true;
}

// Modified from the code above. Checks if the string is a valid float.
bool isFloat(string v) {
    bool no_dot = true;
    for (int i = 0; i < v.length(); i++) {
        if (!isdigit(v[i])) {
            if (v[i] == '.' && no_dot) no_dot = false;
            else return false;
        }

    }

    return true;
}

template <typename T>
TaskQueue<T>::TaskQueue(size_t max_size) {
    pthread_mutex_init(m, NULL);
    sem_init(empty_spaces, 0, max_size);
    sem_init(full_spaces, 0, 0);

    size = 0;
    capacity = max_size;

    queue = new T[max_size];
    cur_pos = 0;
}

template <typename T>
TaskQueue<T>::~TaskQueue() {
    pthread_mutex_destroy(m);
    sem_destroy(empty_spaces);
    sem_destroy(full_spaces);

    delete queue;
}

template <typename T>
void TaskQueue<T>::push(T task) {
    sem_wait(empty_spaces);
    pthread_mutex_lock(m);

    size ++;
    cur_pos = (cur_pos + 1) % capacity;
    queue[cur_pos] = task;

    pthread_mutex_unlock(m);
    sem_post(full_spaces);
}

template <typename T>
T TaskQueue<T>::pop() {
    sem_wait(full_spaces);
    pthread_mutex_lock(m);

    T val = queue[cur_pos];

    size --;
    cur_pos = (cur_pos + capacity - 1) % capacity;

    pthread_mutex_unlock(m);
    sem_post(empty_spaces);

    return val;
}