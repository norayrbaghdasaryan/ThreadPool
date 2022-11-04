#ifndef   THREAD_POOL_H
#define   THREAD_POOL_H

#include <iostream>
#include <functional>
#include <exception>
#include <atomic>
#include <memory>
#include <mutex>

#include <pthread.h>
#include <cstring>
#include <cerrno>

#include "atomic_queue.hpp"
#include "pthread_mutex_lock.hpp"

class ThreadPool {
	static constexpr size_t THREAD_COUNT = 500;
public:
	explicit ThreadPool(size_t = THREAD_COUNT);
	~ThreadPool();
public:
	template<typename F>
	void addJob(F&&);
private:
	void initializeThread(size_t);
private:
	static void* threadFunction(void*); 
private:
	// queue for waiting jobs
	AtomicQueue<std::function<void()>> _queue;
	// threads vector
	std::vector<pthread_t> _threads;
	// conditional variable for idle threads sleep until there appears any job for them
	pthread_cond_t  _cv{};
	// mutex object for _cv conditional variable
	pthread_mutex_t _mutex{};
	std::atomic<bool> _isDestroyed;
};

/**
 * Adds passed functor in waiting queue
 * Once there will be idle thread, functor will be executed asynchronously on posix thread
 * Class does not provide result returning or exception handling mechanism
 * Caller should take care of those things themselves in passed functor
 * @param f - job functor, parameter is caught with universal reference for perfect forwarding
 */
template<typename F>
void ThreadPool::addJob(F&& f) {
    PthreadMutexLock lock(&_mutex);
    _queue.push(std::forward<F>(f));
    // wake up one of waiting threads
    pthread_cond_signal(&_cv);
}

#endif // THREAD_POOL_H