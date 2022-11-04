#ifndef   MUTEX_LOCK_H
#define   MUTEX_LOCK_H

#include <pthread.h>
#include <iostream>

/**
 * PthreadMutexLock class
 * Uses RAII idiom for pthread mutex kernel objects
 * Provides mutex locking for several area of visibility
 */
class PthreadMutexLock {
public:
    /**
     * Locks passed mutex, which will be released in destructor
     * @param mutex mutex
     */
    explicit PthreadMutexLock(pthread_mutex_t *mutex);

    /***
     * Adopts passed mutex (doesn't acquires the mutex)
     * Expects already acquired mutex, which will be released in destructor
     * @param mutex mutex
     */
    [[maybe_unused]] PthreadMutexLock(pthread_mutex_t *mutex, std::adopt_lock_t);

    /**
     * Releases the mutex
     */
    ~PthreadMutexLock();

public:
    // delete copy and move constructors and also assignment operator
    PthreadMutexLock(const PthreadMutexLock &) = delete;

    PthreadMutexLock(PthreadMutexLock &&) = delete;

    PthreadMutexLock &operator=(const PthreadMutexLock &) = delete;

    PthreadMutexLock &operator=(PthreadMutexLock &&) = delete;

private:
    pthread_mutex_t *_mutex;
};

#endif // MUTEX_LOCK_H