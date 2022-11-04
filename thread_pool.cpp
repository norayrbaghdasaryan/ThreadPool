#include "thread_pool.hpp"

namespace {
    char* getLastError() {
        return strerror(errno);
    }
}

ThreadPool::ThreadPool(size_t threadCount)
        : _threads(threadCount), _isDestroyed(false) {
    // initialize mutex
    if (pthread_mutex_init(&_mutex, nullptr) != 0) {
        throw std::runtime_error(getLastError());
    }
    // initialize condvar
    if (pthread_cond_init(&_cv, nullptr) != 0) {
        throw std::runtime_error(getLastError());
    }

    for (size_t i = 0; i < threadCount; ++i) {
        // initialize thread with their handlers
        initializeThread(i);
    }
}

ThreadPool::~ThreadPool() {
    // critical section for mutex
    {
        // lock mutex
        PthreadMutexLock lock(&_mutex);
        // using _isDestroyed flag threads will know they should end their work
        _isDestroyed = true;
        // wake up all waiting threads
        pthread_cond_broadcast(&_cv);
    }
    for (auto& _thread : _threads) {
        // join threads
        pthread_join(_thread, nullptr);
    }
    // destroy mutex
    pthread_mutex_destroy(&_mutex);
    // destroy condvar
    pthread_cond_destroy(&_cv);
}

void ThreadPool::initializeThread(size_t i) {
    pthread_create(&_threads[i], nullptr, &ThreadPool::threadFunction, this);
}

/**
 * Handler function for all threads
 * @param arg - ThreadPool pointer
 */
void *ThreadPool::threadFunction(void *arg) {
    // current thread pool
    ThreadPool &pool = *static_cast<ThreadPool *>(arg);
    std::optional<std::function<void()>> opt;
    while (true) {
        {
            // lock mutex
            PthreadMutexLock lock(&pool._mutex);
            // wait until new job appears or thread pool destroys
            while (!(opt = pool._queue.pop()) && !pool._isDestroyed) {
                pthread_cond_wait(&pool._cv, &pool._mutex);
            }
        }
        // execute all current available jobs
        while(opt) {
            opt.value()();
            opt = pool._queue.pop();
        }
        // Finalize thread execution thread pool destructor is called
        if (pool._isDestroyed) {
            return nullptr;
        }
    }
}