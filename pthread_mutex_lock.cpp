#include "pthread_mutex_lock.hpp"

PthreadMutexLock::PthreadMutexLock(pthread_mutex_t *mutex)
        : _mutex(mutex) {
    pthread_mutex_lock(_mutex);
}

[[maybe_unused]] PthreadMutexLock::PthreadMutexLock(pthread_mutex_t *mutex, std::adopt_lock_t)
        : _mutex(mutex) {}

PthreadMutexLock::~PthreadMutexLock() {
    pthread_mutex_unlock(_mutex);
}