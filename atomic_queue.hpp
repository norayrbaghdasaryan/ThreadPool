#ifndef   ATOMIC_QUEUE_H
#define   ATOMIC_QUEUE_H

#include <pthread.h>
#include <cstring>
#include <cerrno>

#include <stdexcept>
#include <optional>
#include <queue>

#include "pthread_mutex_lock.hpp"

/*
 * Provides atomic interface for queue
 */
template<typename T>
class AtomicQueue {
public:
	AtomicQueue();
	
	~AtomicQueue();
public:
	// delete copy and move constructors and also assignment operator
	AtomicQueue(const AtomicQueue&) = delete;
	AtomicQueue(AtomicQueue&&) = delete;
	AtomicQueue& operator=(const AtomicQueue&) = delete;
	AtomicQueue& operator=(AtomicQueue&&) = delete;
public:
	template<typename... Args>
	void emplace(Args&&...);
	
	void push(const T&);
	
	void push(T&&);
	
public:
	std::optional<T> pop();
	
	void clear();

public:
	bool empty() const;
private:
	std::queue<T> _queue;
	// mutex for providing atomic operations
	mutable pthread_mutex_t _mutex{};
};

template<typename T>
AtomicQueue<T>::AtomicQueue() {
	// initialize mutex
	if (pthread_mutex_init(&_mutex, nullptr) != 0) {
		throw std::runtime_error(strerror(errno));
	}
}

template<typename T>
AtomicQueue<T>::~AtomicQueue() {
	// destroy mutex object
	pthread_mutex_destroy(&_mutex);
}

/*
 * Catch arguments with universal reference and pass to queue using perfect forwarding technique
 * @param args constructor arguments for new queue element
 */
template<typename T>
template<typename... Args>
void AtomicQueue<T>::emplace(Args&&... args) {
	PthreadMutexLock lock(&_mutex);
	_queue.emplace(std::forward<Args>(args)...);
}

template<typename T>
void AtomicQueue<T>::push(const T& value) {
	PthreadMutexLock lock(&_mutex);
	_queue.push(value);
}

template<typename T>
void AtomicQueue<T>::push(T&& value) {
	PthreadMutexLock lock(&_mutex);
	_queue.push(value);
}

/*
 * Pop function for queue
 * If queue is not empty returns optional object containing front value
 * Otherwise object with no value
 * @returns std::optional queue front element
 */
template<typename T>
std::optional<T> AtomicQueue<T>::pop() {
	PthreadMutexLock lock(&_mutex);
	
	std::optional<T> opt;
	if(!_queue.empty()) {
		opt = _queue.front();
		_queue.pop();
	}
	return opt;
}

template<typename T>
void AtomicQueue<T>::clear() {
	PthreadMutexLock lock(&_mutex);
	_queue = std::queue<T>();
}

template<typename T>
bool AtomicQueue<T>::empty() const {
	PthreadMutexLock lock(&_mutex);
	return _queue.empty();
}

#endif // ATOMIC_QUEUE_H
